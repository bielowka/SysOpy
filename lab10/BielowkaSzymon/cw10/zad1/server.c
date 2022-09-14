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
                    send(clients[i]->fd, "ping: ", MSG_LEN, 0);
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

    int port = atoi(argv[1]);
    char* socket_path = argv[2];


    int network_socket = socket(AF_INET,SOCK_STREAM,0);
    if (network_socket  == -1){
        printf("Cannot create network socket!!!\n");
        exit(1);
    }
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
//    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (bind(network_socket,(struct sockaddr *) &sa,sizeof(sa)) < 0){
        printf("Cannot bind network socket!!!\n");
    }
    if (listen(network_socket,MAX_CLIENTS)){
        printf("Cannot start socket!!!\n");
        exit(1);
    }

    int local_socket = socket(AF_UNIX,SOCK_STREAM,0);
    if (local_socket  == -1){
        printf("Cannot create local socket!!!\n");
        exit(1);
    }
    struct sockaddr_un su;
    su.sun_family = AF_UNIX;
    strcpy(su.sun_path, socket_path);
    unlink(socket_path);
    if (bind(local_socket,(struct sockaddr *) &su,sizeof(su)) < 0){
        printf("Cannot bind local socket!!!\n");
        printf("(%s)\n", strerror(errno));
        exit(1);
    }
    if (listen(local_socket,MAX_CLIENTS)){
        printf("Cannot start socket!!!\n");
        exit(1);
    }

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
        if (client_fd == local_socket || client_fd == network_socket){
            client_fd = accept(client_fd, NULL, NULL);
        }
        free(fds);

        char buffer[MSG_LEN + 1];
        recv(client_fd, buffer, MSG_LEN, 0);
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
                send(client_fd, "add:name_taken",MSG_LEN, 0);
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
                if (id % 2 == 1){
                    clients[id-1]->rival = id;
                    new_client->rival = id-1;
                }
                clients[id] = new_client;
            }

            if (id == -2){
                send(client_fd, "add:name_taken",MSG_LEN, 0);
                close(client_fd);
            }
            if (id == -1){
                send(client_fd, "add:name_taken",MSG_LEN, 0);
                close(client_fd);
            }
            else if (id % 2 == 0){
                send(client_fd, "add:no_rival",MSG_LEN, 0);
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

                send(clients[tick]->fd, "add:X",MSG_LEN, 0);
                send(clients[oo]->fd, "add:o",MSG_LEN, 0);
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
            send(clients[clients[player]->rival]->fd, buffer, MSG_LEN,0);

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