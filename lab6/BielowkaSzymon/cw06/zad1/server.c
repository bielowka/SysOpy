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

#include "utils.h"

int server_queue;
int clients[MAX_CLIENTS];


void exit_handler(){
    struct Message stopmsg;
    stopmsg.type = STOP;
    stopmsg.size = 5;

    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] != 0){
            if (msgsnd(clients[i],&stopmsg,msg_size,0) == -1){
                printf("Error - STOP message client %d\n",i);
            }
        }
    }
    sleep(2);
    if (msgctl(server_queue, IPC_RMID, NULL) == -1) {
        printf("Error - cannot remove server queue\n");
    }
}

void sigint_handler(){
    printf("Finished\n");
    exit_handler();
    sleep(2);
    exit(0);
}

void stop_handler(struct Message msg){
    clients[msg.sender] = 0;
}

void list_handler(struct Message msg){
    struct Message active;
    active.type = LIST;
    active.sender = msg.sender;
    char *buffer = calloc(MSG_SIZE,sizeof(char));
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] != 0){
            char *tmp = calloc(5,sizeof(char));
            sprintf(tmp,"%d;",i);
            strcat(buffer,tmp);
        }
    }
    strcat(buffer,"\0");
    active.size = strlen(buffer);
    strcpy(active.content,buffer);

    if (msgsnd(clients[msg.sender],&active,msg_size,0) == -1){
        printf("Error - list message \n");
    }

    free(buffer);
}

void toall_handler(struct Message msg){
    struct Message toallmsg;
    toallmsg.type = TOALL;
    toallmsg.sender = msg.sender;
    toallmsg.size = msg.size;
    strcpy(toallmsg.content,msg.content);
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] != 0){
            toallmsg.time = time(NULL);
            if (msgsnd(clients[i],&toallmsg,msg_size,0) == -1){
                printf("Error - to all message \n");
            }
        }
    }
}

void toone_handler(struct Message msg){
    struct Message toonemsg;
    toonemsg.type = TOONE;
    toonemsg.sender = msg.sender;
    toonemsg.receiver = msg.receiver;
    toonemsg.size = msg.size;
    strcpy(toonemsg.content,msg.content);

    if (clients[msg.receiver] == 0) {
        printf("Error - receiver does not exist \n");
        return;
    }
    toonemsg.time = time(NULL);
    if (msgsnd(clients[msg.receiver],&toonemsg,msg_size,0) == -1){
        printf("Error - to all message \n");
    }
}

void init_handler(struct Message msg){
    int index = -1;
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] == 0){
            index = i;
            break;
        }
    }
    if (index == -1){
        printf("Error - too much clients\n");
        return;
    }


    key_t key = strtol(msg.content, NULL, 10);
    clients[index] = msgget(key, 0);
    if (clients[index] == -1) {
        printf("Error - queue of %d cannot be opened\n",index);
        return;
    }

    struct Message confirmation;
    confirmation.type = INIT;
    confirmation.receiver = index;
    if (msgsnd(clients[index], &confirmation, msg_size, 0) == -1) {
        printf("Error - confirmation message (Server)\n");
        return;
    }

    printf("New client: %d registered with key: %d\n",confirmation.receiver,key);

}

void default_handler(struct Message msg){
    printf("Error - unknown message type (server) \n");
    printf("type of msg unknown %ld \n", msg.type);
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

    server_queue = msgget(key, IPC_CREAT | 0666);

    if (server_queue == -1){
        printf("Error - server queue \n");
        return -1;
    }

    FILE *log = fopen("./log.txt", "a");

    printf("SERVER KEY: %d \n",key);
    fprintf(log,"SERVER KEY: %d \n\n",key);

    char* timer = calloc(21,sizeof(char));

    for (int i = 0; i < MAX_CLIENTS; i++) clients[i] = 0;
    struct Message msg;
    while(1){
        if (msgrcv(server_queue,&msg,msg_size,-6,0) == -1){
            printf("Error - receiving \n");
            return -1;
        }

        strftime(timer, 21, "%Y-%m-%d %H:%M:%S", localtime(&msg.time));

        printf("New message: type: %ld sender: %d receiver: %d at: %s\n",msg.type,msg.sender,msg.receiver,timer);
        fprintf(log,"Time: %s\tClient ID: %d\tContent: %s\n",timer,msg.sender,msg.content);

        switch (msg.type) {
            case STOP:
                stop_handler(msg);
                break;

            case LIST:
                list_handler(msg);
                break;

            case TOALL:
                toall_handler(msg);
                break;

            case TOONE:
                toone_handler(msg);
                break;

            case INIT:
                init_handler(msg);
                break;

            default:
                default_handler(msg);
                break;


        }

    }
    free(timer);
    fclose(log);

}