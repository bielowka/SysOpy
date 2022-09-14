#ifndef UTILS_H
#define UTILS_H

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
#include <mqueue.h>
#include <fcntl.h>

#define MSG_SIZE 1024
#define INFO_SIZE 4
#define PATH "/server"
#define ID 1234
#define MAX_CLIENTS 10
#define CLIENT_ID getpid()


#define STOP 1
#define LIST 2
#define TOALL 3
#define TOONE 4
#define INIT 5

struct Message {
    size_t size;
    time_t time;
    char content[MSG_SIZE]; //0 - type; 1 - sender; 2 - receiver;
//    char sender[INFO_SIZE];
//    char receiver[INFO_SIZE];
};

#define msg_size sizeof(struct Message) - sizeof(long int)

#endif