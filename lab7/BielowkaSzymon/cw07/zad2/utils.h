#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
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
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


#define table_size 5
#define oven_size 5


char table_sem_name[] = "/tablesemaphore";
char oven_sem_name[] = "/ovensemaphore";

char table_mem_name[] = "/tablememory";
char oven_mem_name[] = "/ovenmemory";

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



sem_t *new_semaphores(char * name){
    sem_t *id = sem_open(name, O_CREAT | O_EXCL | O_RDWR, 0666,  1);
    if (id == SEM_FAILED){
        printf("Noncritical error - semaphores exist \n");
        id = sem_open(name, 0);
        if (id != SEM_FAILED) {
            sem_unlink(name);
            sem_close(id);
            id = sem_open(name, O_CREAT | O_EXCL | O_RDWR, 0666,  1);
        }
    }
    if (id == SEM_FAILED){
        printf("Error - semaphores cannot be created \n");
        exit(0);
    }

    return id;
}

int new_memory(char *name){
    int id = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (id == -1){
        printf("Noncritical error - memory exists \n");
        id = shm_open(name, 0, 0666);
        if (id != -1) {
            shm_unlink(name);
            id = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0666);
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