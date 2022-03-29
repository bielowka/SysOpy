#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    for (int i = 0; i < atoi(argv[1]); i++) {
        vfork();
        printf("PID: %d\n", (int)getpid()); //segmentation ???
    }

    return 0;
}