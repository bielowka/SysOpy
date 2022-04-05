#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Wrong number of arguments");
        return -1;
    }

    for (int i = 0; i < atoi(argv[1]); i++) {
        pid_t a = fork();
        if (a == 0){
            printf("PID: %d\n",(int)getpid());
            return 0;
        }
    }
}