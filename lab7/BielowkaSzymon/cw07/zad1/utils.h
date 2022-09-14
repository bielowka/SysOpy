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
#include <stdbool.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define table_size 5
#define oven_size 5

#define TABLE 0
#define OVEN 1

#define table_key 10100
#define oven_key 10101

typedef struct{
    int next;
    int storage[oven_size];
    int contents;
}Oven;

typedef struct{
    int next;
    int storage[table_size];
    int contents;
}Table;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
} arg;

key_t sem_key = 0101;

int new_semaphores(){
    int id = semget(sem_key,2,IPC_CREAT | IPC_EXCL | 0666);
    if (id == -1){
        printf("Noncritical error - semaphores exist \n");
        id = semget(sem_key,2,0666);
        if (id != -1) {
            semctl(id, 0, IPC_RMID);
            id = semget(sem_key,2,IPC_CREAT | IPC_EXCL | 0666);
        }
    }
    if (id == -1){
        printf("Error - semaphores cannot be created \n");
        return -1;
    }

    return id;
}

int new_memory(size_t mem_size,key_t mem_key){
    int id = shmget(mem_key,mem_size,IPC_CREAT | IPC_EXCL | 0666);
    if (id == -1){
        printf("Noncritical error - memory exists \n");
        id = shmget(mem_key,0,0666);
        if (id != -1) {
            shmctl(id, IPC_RMID, NULL);
            id = shmget(sem_key,mem_size,IPC_CREAT | IPC_EXCL | 0666);
        }
    }
    if (id == -1){
        printf("Error - memory cannot be created \n");
        return -1;
    }

    return id;
}

unsigned long get_time(){
    struct timespec t;
    clock_gettime(1, &t);
    return t.tv_sec*1000 + t.tv_nsec / 1000000;
}

#endif