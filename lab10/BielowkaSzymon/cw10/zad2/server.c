#define _POSIX_C_SOURCE 200112L
#include "utils.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

client *clients[MAX_CLIENTS];


void* ping(void* args) {
    while(true) {
        sleep(PING);
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] != NULL) {
                if (clients[i]->active) {
                    clients[i]->active = false;
                    sendto(clients[i]->fd, "ping: ", MSG_LEN, 0, &clients[i]->addr, sizeof(struct addrinfo));
                } else {
                    int oponent = clients[i]->rival;
                    free(clients[i]->nick);
                    free(clients[i]);
                    clients[i] = NULL;

                    if (clients[oponent] != NULL) {
                        free(clients[oponent]->nick);
                        free(clients[oponent]);
                        clients[oponent] = NULL;
                    }
                }
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}


int main(int argc, char *argv[]){
    if (argc != 3) {
        printf("Wrong number of arguments!!!\n");
        exit(0);
    }

//    int port = atoi(argv[1]);
    char* socket_path = argv[2];


    struct addrinfo *info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, argv[1], &hints, &info);
    int network_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    bind(network_socket, info->ai_addr, info->ai_addrlen);
    listen(network_socket, MAX_CLIENTS);
    freeaddrinfo(info);

    int local_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    struct sockaddr_un local_sockaddr;
    memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
    local_sockaddr.sun_family = AF_UNIX;
    strcpy(local_sockaddr.sun_path, socket_path);
    unlink(socket_path);
    bind(local_socket, (struct sockaddr *)&local_sockaddr,
         sizeof(struct sockaddr_un));
    listen(local_socket, MAX_CLIENTS);

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, ping, NULL);


    while (true){
        struct pollfd *fds = calloc(MAX_CLIENTS + 2, sizeof(struct pollfd));
        fds[0].fd = local_socket;
        fds[0].events = POLLIN;
        fds[1].fd = network_socket;
        fds[1].events = POLLIN;

        pthread_mutex_lock(&mutex);

        for (int i = 2; i < MAX_CLIENTS + 2; i++){
            if (clients[i - 2] == NULL) break;
            fds[i].fd = clients[i - 2]->fd;
            fds[i].events = POLLIN;
        }
        pthread_mutex_unlock(&mutex);

        poll(fds, MAX_CLIENTS + 2, -1);
        int client_fd = -1;
        for (int i = 0; i < MAX_CLIENTS + 2; i++){
            if (fds[i].revents & POLLIN){
                client_fd = fds[i].fd;
                break;
            }
        }
        free(fds);

        char buffer[MSG_LEN + 1];
        struct sockaddr from_addr;
        socklen_t from_length = sizeof(struct sockaddr);
        recvfrom(client_fd, buffer, MSG_LEN, 0, &from_addr, &from_length);


        char *command = strtok(buffer, ":");
        char *arg = strtok(NULL, ":");
        char *nick = strtok(NULL, ":");



        pthread_mutex_lock(&mutex);
        int id = -1;
        if (strcmp(command, "add") == 0){
            for (int i = 0; i < MAX_CLIENTS; i++){
                if (clients[i] != NULL && strcmp(clients[i]->nick, nick) == 0) {
                    id = -2;
                    break;
                }
            }

            if (id == -2){
                sendto(client_fd, "add:name_taken", MSG_LEN, 0, (struct sockaddr *)&from_addr, sizeof(struct addrinfo));
                continue;
            }

            for (int i = 0; i < MAX_CLIENTS; i = i + 2){
                if (clients[i] != NULL && clients[i + 1] == NULL){
                    id = i + 1;
                    break;
                }
            }
            if (id == -1){
                for (int i = 0; i < MAX_CLIENTS; i++){
                    if (clients[i] == NULL){
                        id = i;
                        break;
                    }
                }
            }

            if (id > -1){
                client *new_client = calloc(1, sizeof(client));
                new_client->fd = client_fd;
                new_client->nick = calloc(MSG_LEN, sizeof(char));
                strcpy(new_client->nick,nick);
                new_client->active = true;
                new_client->addr = from_addr;
                if (id % 2 == 1){
                    clients[id-1]->rival = id;
                    new_client->rival = id-1;
                }
                clients[id] = new_client;
            }

            if (id == -2){
                sendto(client_fd, "add:name_taken", MSG_LEN, 0, (struct sockaddr *)&from_addr, sizeof(struct addrinfo));
                close(client_fd);
            }
            if (id == -1){
                sendto(client_fd, "add:name_taken", MSG_LEN, 0, (struct sockaddr *)&from_addr, sizeof(struct addrinfo));
                close(client_fd);
            }
            else if (id % 2 == 0){
                sendto(client_fd, "add:no_rival", MSG_LEN, 0, (struct sockaddr *)&from_addr, sizeof(struct addrinfo));
            }
            else{
                srand(time(NULL));
                int which_symbol = rand() % 2;
                int tick, oo;

                if (which_symbol == 0){
                    tick = id;
                    oo = id - 1;
                }
                else{
                    tick = id - 1;
                    oo = id;
                }

                sendto(clients[tick]->fd, "add:X",MSG_LEN, 0, &clients[tick]->addr, sizeof(struct addrinfo));
                sendto(clients[oo]->fd, "add:o",MSG_LEN, 0, &clients[oo]->addr, sizeof(struct addrinfo));
            }
        }
        else if (strcmp(command, "move") == 0){
            int move = atoi(arg);

            int player = -1;
            for (int i = 0; i < MAX_CLIENTS; i++){
                if (clients[i] != NULL && strcmp(clients[i]->nick, nick) == 0) {
                    player = i;
                    break;
                }
            }

            sprintf(buffer, "move:%d", move);
            sendto(clients[clients[player]->rival]->fd, buffer, MSG_LEN,0, &clients[clients[player]->rival]->addr, sizeof(struct addrinfo));

        }
        else if (strcmp(command, "end") == 0){
            for (int i = 0; i < MAX_CLIENTS; i++){
                if (clients[i] != NULL && strcmp(clients[i]->nick,nick) == 0){
                    int oponent = clients[i]->rival;
                    free(clients[i]->nick);
                    free(clients[i]);
                    clients[i] = NULL;

                    if (clients[oponent] != NULL){
                        free(clients[oponent]->nick);
                        free(clients[oponent]);
                        clients[oponent] = NULL;
                    }

                }
            }
        }
        else if (strcmp(command, "reply") == 0){
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] != NULL && strcmp(clients[i]->nick, nick) == 0) {
                    clients[i]->active = true;
                }
            }
        }
        pthread_mutex_unlock(&mutex);
    }
//
//
//
//

}