#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <sys/file.h>


int main(int argc, char* argv[]){
    if (argc != 4){
        printf("Wrong number of arguments!!!!");
        return 0;
    }

    char* fdpath = argv[1];
    char* filepath = argv[2];
    int n = atoi(argv[3]);

    FILE* fd = fopen(fdpath, "r");
    FILE* destination = fopen(filepath, "r+");

    char *buffer = calloc(n+3, sizeof(char));

    int read;
    while ((read = fread(buffer,sizeof(char),n+3,fd)) > 0){

        char *pos = strtok(buffer,":");

        int row = atoi(pos);

        pos = strtok(NULL,":");

        fseek(destination,0,SEEK_SET);
        int fds = fileno(destination);
        flock(fds, LOCK_EX);


        char *buff = calloc(256,sizeof(char));

        int i = 0;
        int read2;
        while ((read2 = fread(buff,sizeof(char),256,destination)) > 0) {
            if (i == row) {
                int j = 0;
                while(j < 256){
                    if( buff[j] == ' ') break;
                    j++;
                }
                printf("\n%s %d %d %c\n",pos,i,j,buff[j]);
                fseek(destination,row * 256 + j,SEEK_SET);
                fprintf(destination,pos,strlen(pos)+1);
                fflush(destination);
                break;
            }
            i++;
        }
        flock(fds, LOCK_UN);

    }
    fclose(fd);
    fclose(destination);

}