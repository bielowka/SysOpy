#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char **argv){
    if (strcmp(argv[1], "pending") == 0) {
        sigset_t pending;
        sigpending(&pending);
        printf("Signal pending in child in exec %d\n",sigismember(&pending, SIGUSR1));
    }
    else{
        if (strcmp(argv[1], "mask") == 0) {
            raise(SIGUSR1);
            sigset_t pending;
            sigpending(&pending);
            printf("Signal pending in child in exec %d\n", sigismember(&pending, SIGUSR1));
        }
    }
}