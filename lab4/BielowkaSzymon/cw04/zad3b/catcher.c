#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


int n = 0;
int mode = 0;
int SIG1 = SIGUSR1;
int SIG2 = SIGUSR2;
sigset_t mask;

void handler(int sig, siginfo_t *info, void *context){
    if (sig == SIGUSR1 || sig == SIGRTMIN + 1) {
        printf("Catched %d from %d",sig,info->si_pid);
        if (info->si_code == SI_QUEUE) {
            printf(" - val: %d", info->si_value.sival_int);
        }
        printf("\n");
        n++;
        if (mode == 2){
            union sigval val;
            sigqueue(info->si_pid, SIGUSR1, val);
        }
        else {
            kill(info->si_pid, SIGUSR1);
        }
        sigsuspend(&mask);
    }
    else if (sig == SIGUSR2 || sig == SIGRTMIN + 2){
        pid_t sender = info->si_pid;
        if (mode == 0 || mode == 2){
            kill(sender, SIGUSR2);
        }
        else if (mode == 1) {
            union sigval val;
            sigqueue(sender,SIGUSR2,val);
        }
        printf("%d signals catched\n",n);
        exit(0);
        return;
    }
}

int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Wrong number of arguments\n");
        return -1;
    }

    printf("Catcher PID: %d\n",getpid());


    if (strcmp(argv[1],"KILL")==0){
        mode = 0;
        SIG1 = SIGUSR1;
        SIG2 = SIGUSR2;
    }
    else if (strcmp(argv[1],"SIGQUEUE")==0){
        mode = 1;
    }
    else if (strcmp(argv[1],"SIGRT")==0){
        mode = 2;
        SIG1 = SIGRTMIN + 1;
        SIG2 = SIGRTMIN + 2;
    }
    else {
        printf("Mode unrecognised\n");
        return -1;
    }


    sigfillset(&mask);
    sigdelset(&mask, SIG1);
    sigdelset(&mask, SIG2);
    sigprocmask(SIG_SETMASK, &mask, NULL);

//    sigset_t inmask;
//    sigfillset(&inmask);

    struct sigaction action;
    action.sa_flags = SA_SIGINFO;
//    action.sa_mask = inmask;
    action.sa_mask = mask;
    action.sa_sigaction = handler;

    sigaction(SIG1, &action, NULL);
    sigaction(SIG2, &action, NULL);

    sigsuspend(&mask);
    exit(0);
}