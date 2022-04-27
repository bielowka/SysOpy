#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>

void resultfile(char *name, int len){
    FILE* file = fopen(name, "w");
    char* line = calloc(256, sizeof(char));

    for (int j = 0; j < 255; j++){
        line[j] = ' ';
    }
    line[255] = '\n';

    for (int i = 0; i < len-1; i++){
        fprintf(file, "%s", line);
    }
    line[255] = '\0';
    fprintf(file, "%s", line);
    fclose(file);
    free(line);
}

void MtoOne(){
    int prods = 5;
    char* prodfiles[] = {"A.txt", "B.txt", "C.txt", "D.txt", "E.txt"};

    resultfile("result.txt",5);

    pid_t a = fork();
    if (a == 0){
        execl("./consumer", "./consumer", "fd", "result.txt", "4", NULL);
    }

    for (int i = 0; i < prods; i++){
        a = fork();

        char row[2];
        sprintf(row,"%d", i);

        if (a == 0){
            execl("./producer", "./producer", "fd", row, prodfiles[i], "4",NULL);
        }
    }

    while(wait(NULL)>0);



}

void OnetoM(){

}

void MtoM(){

}

int main(int argc, char* argv[]){
    mkfifo("fd", 0666);

    MtoOne();



}