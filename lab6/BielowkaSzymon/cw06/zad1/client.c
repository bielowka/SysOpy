#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <sys/file.h>
#include <time.h>
#include <sys/times.h>
#include <errno.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdbool.h>

#include "utils.h"

int server_queue;
int client_queue;
int cl_id;


void exit_handler(){
    if (msgctl(client_queue, IPC_RMID, NULL) == -1) {
        printf("Error - cannot remove client queue\n");
    }
}

void send_stop(){
    struct Message stopmsg;
    stopmsg.sender = cl_id;
    stopmsg.type = STOP;
    if (msgsnd(server_queue, &stopmsg, msg_size, 0) == -1) {
        printf("Error - STOP cannot be sent\n");
    }
    exit(0);
}

void sigint_handler(){
    send_stop();
}

void send_list(){
    struct Message msg;
    msg.sender = cl_id;
    msg.type = LIST;
    msg.time = time(NULL);

    if (msgsnd(server_queue, &msg, msg_size, 0) == -1) {
        printf("Error - LIST cannot be sent\n");
    }
    struct Message answer;
    if (msgrcv(client_queue, &answer, msg_size, LIST, 0) == -1) {
        printf("Error - LIST cannot be received\n");
    }
    printf("Client num: %d\n clients: %s\n", cl_id, answer.content);
}

void send_toall(char *val){
    struct Message msg;
    msg.sender = cl_id;
    msg.type = TOALL;
    msg.time = time(NULL);

    strcpy(msg.content, val);

    if (msgsnd(server_queue, &msg,msg_size, 0) == -1) {
        printf("Error - TOALL cannot be sent\n");
    }
}

void send_toone(int user_id, char *val){
    struct Message msg;
    msg.sender = cl_id;
    msg.type = TOONE;
    msg.receiver = user_id;
    msg.time = time(NULL);


    strcpy(msg.content, val);
    if (msgsnd(server_queue, &msg, msg_size, 0) == -1) {
        printf("Error - TOONE cannot be sent\n");
    }
}

void toall_handler(struct Message message){
    printf("I am: %d\n Message: %s\n\n", cl_id,message.content);
}

void toone_handler(struct Message message){
    printf("I am: %d\n Message: %s\n From: %d\n\n", cl_id,message.content,message.sender);
}

void default_handler(struct Message message){
    printf("unknown message type \n");
    printf("type of msg unknown %ld \n", message.type);
}


int main(){
    if (atexit(exit_handler) == -1) {
        printf("Error - exit handler \n");
        return -1;
    }

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        printf("Error - sigint handler \n");
        return -1;
    }

    key_t key = ftok(PATH, ID);

    if (key == -1){
        printf("Error - key \n");
        return -1;
    }

    server_queue = msgget(key, 0);

    if (server_queue == -1){
        printf("Error - server queue \n");
        return -1;
    }

    char *path = getenv("HOME");
    if (path == NULL) {
        printf("Error - home path \n");
    }
    printf("path: %s\n", path);
    key_t clkey = ftok(path, CLIENT_ID);
    printf("KEY: %d \n",clkey);
    if (clkey == -1){
        printf("Error - key \n");
        return -1;
    }

    client_queue = msgget(clkey, IPC_CREAT | IPC_EXCL | 0666);

    if (client_queue == -1){
        printf("Error - client queue \n");
        return -1;
    }

    struct Message request;
    request.type = INIT;
    sprintf(request.content, "%d", clkey);
    request.time = time(NULL);
    if (msgsnd(server_queue, &request, msg_size, 0) == -1) {
        printf("Error - initial message\n");
    }

    struct Message confirmation;
    if (msgrcv(client_queue, &confirmation, msg_size, INIT, 0) == -1) {
        printf("Error - confirmation message\n");
    }


    cl_id = confirmation.receiver;

    printf("I am %d\n",cl_id);

    struct msqid_ds queue;
    struct Message message;
    char buffer[MSG_SIZE+20];


    while(1){
        if (msgctl(client_queue, IPC_STAT, &queue)) {
            printf("Error - queue information (client)\n");
            exit(0);
        }
        if (queue.msg_qnum > 0){
            if (msgrcv(client_queue, &message, msg_size, 0, 0) == -1) {
                printf("Error - message from client\n");
                exit(0);
            }
            switch (message.type) {
                case STOP:
                    send_stop();
                    break;
                case TOALL:
                    toall_handler(message);
                    break;
                case TOONE:
                    toone_handler(message);
                    break;
                default:
                    default_handler(message);
                    break;
            }
            
        }
        else{
            struct timeval t;
            fd_set fds;
            t.tv_sec = 1;
            t.tv_usec = 0;
            FD_ZERO(&fds);
            FD_SET(STDIN_FILENO, &fds);
            select(STDIN_FILENO + 1, &fds, NULL, NULL, &t);
            bool notempty = FD_ISSET(0, &fds);
            if (notempty){
                if (fgets(buffer,MSG_SIZE+20, stdin) == NULL) {
                    printf("Error - input\n");
                    exit(0);
                }
                if (buffer[strlen(buffer)-1] == '\n') buffer[strlen(buffer)-1] = '\0';
                char *val;
                char *type = strtok_r(buffer," \0",&val);
                if (strcmp(type,"STOP") == 0){
                    send_stop();
                }
                else if (strcmp(type,"LIST") == 0){
                    send_list();
                }
                else if (strcmp(type,"TOALL") == 0){
                    send_toall(val);
                }
                else if (strcmp(type,"TOONE") == 0){
                    char *user = strtok_r(NULL, " \0", &val);
                    int user_id = strtol(user, NULL, 10);

                    send_toone(user_id, val);
                }
                else {
                    printf("Error - wrong message\n");
                }
            }
            else {
                sleep(1);
            }
        }
    }



}