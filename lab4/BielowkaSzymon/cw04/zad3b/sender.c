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
    if (sig == SIGUSR1) {
        printf("%d num %d reached %d\n",sig,n,info->si_pid);
        n++;
    }
    else if (sig == SIGUSR2) {
        printf("Number of signals back: %d\n",n);
    }
}


int main(int argc, char *argv[]){
    if (argc != 4) {
        printf("Wrong number of arguments\n");
        return -1;
    }

    pid_t catcher = atoi(argv[1]);
    if (catcher <= 0){
        printf("Wrong PID\n");
        return -1;
    }
    int amount_to_send =atoi(argv[2]);
    if (amount_to_send < 0){
        printf("Cannot send negative number of signals\n");
        return -1;
    }

    if (strcmp(argv[3],"KILL")==0){
        mode = 0;
        SIG1 = SIGUSR1;
        SIG2 = SIGUSR2;
    }
    else if (strcmp(argv[3],"SIGQUEUE")==0){
        mode = 1;
    }
    else if (strcmp(argv[3],"SIGRT")==0){
        mode = 2;
        SIG1 = SIGRTMIN + 1;
        SIG2 = SIGRTMIN + 2;
    }
    else {
        printf("Mode unrecognised\n");
        return -1;
    }



    sigfillset(&mask);
//    sigdelset(&mask, SIG1);
//    sigdelset(&mask, SIG2);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigprocmask(SIG_SETMASK, &mask, NULL);

//    sigset_t in_mask;
//    sigfillset(&in_mask);

    struct sigaction action;
    action.sa_flags = SA_SIGINFO;
//    action.sa_mask = in_mask;
    action.sa_mask = mask;
    action.sa_sigaction = handler;

//    sigaction(SIG1, &action, NULL);
//    sigaction(SIG2, &action, NULL);
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);

    printf("Sending: \n");

    if (mode == 0 || mode == 2){
        for (int i = 0; i < amount_to_send; i++){
            printf("Sent %d to %d\n",SIG1,catcher);
            kill(catcher, SIG1);
            sigsuspend(&mask);
        }
        kill(catcher, SIG2);
        sigsuspend(&mask);
    }
    else if (mode == 1) {
        union sigval val;
        for (int i = 0; i < amount_to_send; i++) {
            val.sival_int = i;
            printf("Sent %d to %d\n",SIG1,catcher);
            sigqueue(catcher, SIG1, val);
            sigsuspend(&mask);
        }
        sigqueue(catcher,SIG2,val);
        sigsuspend(&mask);
    }

    return 0;
}