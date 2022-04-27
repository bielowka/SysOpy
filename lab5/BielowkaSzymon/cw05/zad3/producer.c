#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>

int main(int argc, char* argv[]){
    if (argc != 5){
        printf("Wrong number of arguments!!!!");
        return 0;
    }

    char* fdpath = argv[1];
    int row = atoi(argv[2]);
    char* filepath = argv[3];
    int n = atoi(argv[4]);

    FILE* fd = fopen(fdpath, "w");
    FILE* source = fopen(filepath, "r");

//    char buffer[n];
//    char tosend[n+4];
    char *buffer = calloc(n,sizeof(char));
    char *tosend = calloc(n+4,sizeof(char));

    int read;
    printf("%d\n",n);
    while ((read = fread(buffer,sizeof(char),n,source)) > 0){
        sleep(1);
        printf("Sending: %d:%s\n",row,buffer);
        sprintf(tosend, "%d:%s:",row,buffer);
        fwrite(tosend,sizeof(char),strlen(tosend),fd);
    }

    fclose(fd);
    fclose(source);
    free(buffer);
    free(tosend);
}