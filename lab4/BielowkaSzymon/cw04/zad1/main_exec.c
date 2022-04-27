#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>

void handle(int sigint){
    printf("Process: %d - ",getpid());
    printf("- SIGUSR1 handled \n");
}

int main(int argc, char **argv){
    if (argc != 2){
        printf("Wrong number of arguments!!!");
        return -1;
    }
    if (strcmp(argv[1],"ignore")==0){
        signal(SIGUSR1, SIG_IGN);
    }
    else if (strcmp(argv[1],"handler")==0){
        signal(SIGUSR1,handle);
    }
    else if (strcmp(argv[1],"mask")==0 || strcmp(argv[1],"pending")==0){
        sigset_t newmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        if (sigprocmask(SIG_SETMASK,&newmask, NULL) == -1){
            printf("Can't process mask!!!");
            exit(1);
        }
    }

    raise(SIGUSR1);
    if (strcmp(argv[1],"mask")==0 || strcmp(argv[1],"pending")==0){
        sigset_t pending;
        sigpending(&pending);
        printf("Signal pending in parent %d\n",sigismember(&pending, SIGUSR1));
    }
    execl("./exec","./exec",argv[1],NULL);
    while(wait(NULL) > 0);
}