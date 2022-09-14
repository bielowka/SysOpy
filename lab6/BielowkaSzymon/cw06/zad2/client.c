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
#include <mqueue.h>
#include <fcntl.h>
#include <sys/select.h>

#include "utils.h"



int server_queue;
int client_queue;
int cl_id;


void exit_handler(){
    if (mq_close(server_queue) == -1) {
        printf("Error - cannot close server queue\n");
    }
    if (mq_close(client_queue) == -1) {
        printf("Error - cannot close client queue\n");
    }

    char *path = getenv("HOME");
    if (path == NULL) {
        printf("Error - home path \n");
    }

    char tmp[20];
    strcpy(tmp, path);
    char *ptr;
    strtok_r(tmp, "/", &ptr);
    char name[30];
    name[0] = '/';
    name[1] = '\0';
    strcat(name, ptr);
    char pid[4];
    sprintf(pid,"%d",getpid());
    strcat(name,pid);

    if (mq_unlink(name) == -1) {
        printf("Error - cannot unlinke client queue\n");
    }
}

void send_stop(){
    struct Message stopmsg;
    sprintf(stopmsg.content,"%d%d",STOP,cl_id);
    if (mq_send(server_queue, (char*)&stopmsg, MSG_SIZE, STOP) == -1) {
        printf("Error - STOP cannot be sent\n");
    }
    exit(0);
}

void sigint_handler(){
    send_stop();
}

void send_list(){
    struct Message msg;
    sprintf(msg.content,"%d%d",LIST,cl_id);
    msg.time = time(NULL);

    if (mq_send(server_queue, (char*)&msg, MSG_SIZE, LIST) == -1) {
        printf("Error - LIST cannot be sent\n");
    }
    struct Message answer;
    if (mq_receive(client_queue, (char*)&answer, MSG_SIZE, NULL) == -1) {
        printf("Error - LIST cannot be received\n");
    }
    printf("Client num: %d\n clients: %s\n", cl_id, answer.content+2);
}

void send_toall(char *val){
    struct Message msg;

    msg.time = time(NULL);


    sprintf(msg.content,"%d%s",TOALL,val);

    if (mq_send(server_queue, (char*)&msg, MSG_SIZE, TOALL) == -1) {
        printf("Error - TOALL cannot be sent\n");
    }
}

void send_toone(int user_id, char *val){
    struct Message msg;



    msg.time = time(NULL);



    sprintf(msg.content,"%d%d%d%s",TOONE,cl_id,user_id,val);
    if (mq_send(server_queue, (char*)&msg, MSG_SIZE, TOONE) == -1) {
        printf("Error - TOONE cannot be sent\n");
    }
}

void toall_handler(struct Message message){
    printf("I am: %d\n Message: %s\n\n", cl_id,message.content+1);
}

void toone_handler(struct Message message){
    printf("I am: %d\n Message: %s\n From: %c\n\n", cl_id,message.content+3,message.content[1]);
}

void default_handler(struct Message message){
    printf("unknown message type \n");
    printf("type of msg unknown %c \n", message.content[0]);
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


    server_queue = mq_open(PATH, O_WRONLY);

    if (server_queue == -1){
        printf("Error - server queue \n");
        return -1;
    }



    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_SIZE;

    char *path = getenv("HOME");
    if (path == NULL) {
        printf("Error - home path \n");
    }

    char tmp[20];
    strcpy(tmp, path);

    char *ptr;
    strtok_r(tmp, "/", &ptr);
    char name[30];
    name[0] = '/';
    name[1] = '\0';
    strcat(name, ptr);
    char pid[4];
    sprintf(pid,"%d",getpid());
    strcat(name,pid);

    client_queue = mq_open(name, O_RDONLY | O_CREAT, 0666, &attr);
    if (client_queue == -1){
        printf("Error - client queue \n");
        return -1;
    }

    struct Message request;
    sprintf(request.content, "%d%s", INIT,name);
    request.time = time(NULL);
    if (mq_send(server_queue, (char*)&request, MSG_SIZE, INIT) == -1) {
        printf("Error - initial message\n");
    }

    struct Message confirmation;
    if (mq_receive(client_queue, (char*)&confirmation, MSG_SIZE, NULL) == -1){
        printf("Error - confirmation message\n");
    }

    cl_id = atoi(confirmation.content+1);


    struct mq_attr queue;
    struct Message message;
    char buffer[MSG_SIZE+20];



    while(1){
        if (mq_getattr(client_queue, &queue) == -1) {
            printf("Error - queue information (client)\n");
            exit(0);
        }
        if (queue.mq_curmsgs > 0){
            if (mq_receive(client_queue, (char*)&message, MSG_SIZE, NULL) == -1) {
                printf("Error - message from client\n");
                exit(0);
            }
            switch (message.content[0]-'0') {
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