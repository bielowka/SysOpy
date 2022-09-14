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

#include <mqueue.h>
#include <fcntl.h>

int server_queue;
int clients[MAX_CLIENTS];


void exit_handler(){
    struct Message stopmsg;
    sprintf(stopmsg.content,"%d",STOP);
    stopmsg.size = 5;

    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] != 0){
            if (mq_send(clients[i], (char*)&stopmsg, MSG_SIZE, STOP) == -1){
                printf("Error - STOP message client %d\n",i);
            }
        }
    }
    sleep(2);
    if (mq_close(server_queue) == -1) {
        printf("Error - cannot close server\n");
    }
    if (mq_unlink(PATH) == -1) {
        printf("Error - cannot unlink server\n");
    }
}

void sigint_handler(){
    printf("Finished\n");
    exit_handler();
    sleep(2);
    exit(0);
}

void stop_handler(struct Message msg){
    if (mq_close(clients[msg.content[1]-'0']) == -1) {
        printf("Error - cannot close client queue\n");
        return;
    }
    clients[msg.content[1]-'0'] = 0;
}

void list_handler(struct Message msg){
    struct Message active;

    char *buffer = calloc(MSG_SIZE,sizeof(char));

    sprintf(buffer,"%d%c",LIST,msg.content[1]);
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] != 0){
            char *tmp = calloc(5,sizeof(char));
            sprintf(tmp,"%d;",i);
            strcat(buffer,tmp);
        }
    }
    strcat(buffer,"\0");
    active.size = strlen(buffer);
    sprintf(active.content,"%s",buffer);

    if (mq_send(clients[msg.content[1]-'0'], (char*)&active, MSG_SIZE, LIST) == -1) {
        printf("Error - list message \n");
    }

    free(buffer);
}

void toall_handler(struct Message msg){
    struct Message toallmsg;

    toallmsg.size = msg.size;
    strcpy(toallmsg.content,msg.content);

    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] != 0){
            toallmsg.time = time(NULL);
            if (mq_send(clients[i], (char*)&toallmsg, MSG_SIZE, TOALL) == -1) {
                printf("Error - to all message \n");
            }
        }
    }
}

void toone_handler(struct Message msg){
    struct Message toonemsg;

    toonemsg.size = msg.size;
    strcpy(toonemsg.content,msg.content);

    if (clients[msg.content[2]-'0'] == 0) {
        printf("Error - receiver does not exist \n");
        return;
    }
    toonemsg.time = time(NULL);
    if (mq_send(clients[msg.content[2]-'0'], (char*)&msg, MSG_SIZE, TOONE) == -1){
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



    clients[index] = mq_open(msg.content+1, O_WRONLY);

    if (clients[index] == -1) {
        printf("Error - queue of %d cannot be opened\n",index);
        return;
    }

    struct Message confirmation;
    sprintf(confirmation.content,"%d%d",INIT,index);

    if (mq_send(clients[index], (char*)&confirmation, MSG_SIZE, INIT) == -1)  {
        printf("Error - confirmation message (Server)\n");
        return;
    }

    printf("New client: %c registered\n",confirmation.content[1]);

}

void default_handler(struct Message msg){
    printf("Error - unknown message type (server) \n");
    printf("type of msg unknown %c \n", msg.content[0]);
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


    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_SIZE;
    attr.mq_flags = 0;

    server_queue = mq_open(PATH, O_RDONLY | O_CREAT, 0666, &attr);
    if (server_queue == -1) {
        printf("Error - server queue \n");
        return -1;
    }

    FILE *log = fopen("./log.txt", "a");

    printf("SERVER ready\n");
    fprintf(log,"\n\n\nSERVER\n\n");

    char* timer = calloc(21,sizeof(char));

    for (int i = 0; i < MAX_CLIENTS; i++) clients[i] = 0;
    struct Message msg;
    while(1){
        if (mq_receive(server_queue, (char*)&msg, MSG_SIZE, NULL) == -1){
            printf("Error - receiving \n");
            return -1;
        }

        strftime(timer, 21, "%Y-%m-%d %H:%M:%S", localtime(&msg.time));

        printf("New message: type: %c sender: %c receiver: %c at: %s\n",msg.content[0],msg.content[1],msg.content[2],timer);
        fprintf(log,"New message: type: %c sender: %c receiver: %c at: %s\n",msg.content[0],msg.content[1],msg.content[2],timer);

        switch (msg.content[0] - '0') {
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