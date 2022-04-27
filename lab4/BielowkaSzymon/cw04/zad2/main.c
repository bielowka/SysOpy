#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void handler_one(int sig, siginfo_t *info, void *context){
    printf("Information::  ");
    printf("Number: %d;"
           " Code: %d; "
           " Int value: %d; "
           " UID: %d; "
           " PID: %d; "
           "\n",info->si_signo,info->si_code,info->si_value.sival_int,info->si_uid,info->si_pid);
}

void scenario_one(){
    struct sigaction action;
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = handler_one;

    sigaction(SIGUSR1,&action,NULL);
    sigaction(SIGUSR2,&action,NULL);

    raise(SIGUSR1);
    raise(SIGUSR2);
}

void handler_two(int sig){
    printf("Child %d notified %d\n",getpid(), getppid());
}

void scenario_two(){
    struct sigaction action;
    action.sa_handler = handler_two;
    sigaction(SIGCHLD,&action,NULL);
    printf("No flag: \n");

    pid_t a = fork();
    if (a == 0) {
        while(1>0){};
    }
    else {
        sleep(1);
        kill(a,SIGSTOP);
        printf("\n");
    }

    printf("With flag: \n");
    action.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &action, NULL);

    a = fork();
    if (a == 0) {
        while(1>0){};
    }
    else {
        sleep(1);
        kill(a,SIGSTOP);
        printf("\n");
    }
    printf("<no notification>\n");
}

void handler_three(int sig){
    printf("Handling signal %d in process %d\n", sig, getpid());
}

void scenario_three(){
    struct sigaction action;
    action.sa_handler = &handler_three;
    sigaction(SIGCHLD, &action, NULL);

    printf("Without flag: \n");
    pid_t a;
    for (int i = 0; i < 3; i++) {
        a = fork();
        if (a == 0) {
            while (1 > 0) {};
        } else {
            kill(a, SIGKILL);
            wait(NULL);
        }
    }

    printf("With flag:\n");
    action.sa_flags = SA_RESETHAND;
    sigaction(SIGCHLD, &action, NULL);

    for (int i = 0; i < 3; i++) {
        a = fork();
        if (a == 0) {
            while (1 > 0) {};
        } else {
            kill(a, SIGKILL);
            wait(NULL);
        }
    }
    printf("<handled only once and then ignored as default>\n");
}

int main(){
    printf("---------------------------------------\n");
    printf("Testing SA_SIGINF: \n");
    scenario_one();
    printf("---------------------------------------\n");
    printf("\n");

    printf("---------------------------------------\n");
    printf("Testing SA_NOCLDSTOP: \n");
    scenario_two();
    printf("---------------------------------------\n");
    printf("\n");

    printf("---------------------------------------\n");
    printf("Testing SA_NODEFER: \n");
    scenario_three();
    printf("---------------------------------------\n");
    printf("\n");
}