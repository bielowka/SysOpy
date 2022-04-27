#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>


#define MAX_IN_LINE 10

char** parser(char* line){
    char** commands = (char**) calloc(MAX_IN_LINE, sizeof(char*));

    char* command = strtok(line, "=");
    int i = 0;
    while ((command = strtok(NULL,"|")) != NULL){
        commands[i] = command;
        i++;
    }

    return commands;
}

int countlines(FILE *file){
    int result = 0;
    char c = getc(file);
    while (c != EOF)
    {
        if (c == '\n')
        {
            result++;
        }
        c = getc(file);
    }
    return result;
}


void singlecommandparser(char *command, char* path, char** args){
    char* tmp = strtok(command, " ");
    strcpy(path, tmp);

    int i = 1;
    args[0] = tmp;
    while ((tmp = strtok(NULL," ")) != NULL) {
        args[i] = tmp;
        i++;
    }
    args[i] = NULL;
}


int whichnum(char* command, int i){
    if (i == 0) return command[8] - '0';
    else return command[9] - '0';
}

int* toexecute(char* line){
    int* commands = (int*) calloc(MAX_IN_LINE, sizeof(int));
    char* command = strtok(line,"|");
    commands[0] = whichnum(command,0);

    int i = 1;
    while ((command = strtok(NULL,"|")) != NULL){
        commands[i] = whichnum(command,i);
        i++;
    }
    commands[i] = -1;
    return commands;
}



int main(int argc, char* argv[]) {
    if (argc != 2){
        printf("Wrong number of arguments!!!!\n");
        return 0;
    }

    char* filepath = argv[1];
    FILE *file = fopen(filepath,"r");
    int num = countlines(file);
    fseek(file,0,SEEK_SET);

    char *line = NULL;
    size_t len = 0;

    char** commands = (char**) calloc(256, sizeof(char*));
    int commcount = 0;
    int** toexec = (int**) calloc(MAX_IN_LINE, sizeof(int*));
    int toexeccount = 0;

    for (int i = 0; i < num; i ++){
        getline(&line, &len, file);
        if (strstr(line, "=")) {
            char *tmp = (char*) calloc(256, sizeof(char));
            strcpy(tmp, line);
            tmp = strtok(tmp,"\n");
            commands[commcount] = tmp;
            commcount++;
        }
        else {
            toexec[toexeccount] = toexecute(line);
            toexeccount++;
        }
    }
//    printf("%s \n",commands[toexec[0][0]-1]);
//    printf("%s \n",commands[toexec[0][1]-1]);

    char** commandsin = (char**) calloc(MAX_IN_LINE, sizeof(char*));
    int numtoexec = 0;

    for (int i = 0; i < toexeccount; i++){

            int fd_in[2];
            int fd_out[2];

            pipe(fd_out);

            numtoexec = 0;
            while(toexec[i][numtoexec] != -1) numtoexec++;

            for (int j = 0; j < numtoexec; j++) {
                commandsin = parser(commands[toexec[i][j]-1]);

                int m = 0;
                while (commandsin[m] != NULL) m++;

                for (int k = 0; k < m; k++) {

                    pid_t a = fork();

                    if (a == 0) {

                        if (k == 0 && j == 0) {
                            close(fd_out[0]);
                            dup2(fd_out[1], STDOUT_FILENO);
                        }

                        else if (k == m - 1 && j == numtoexec-1) {
                            close(fd_out[0]);
                            close(fd_out[1]);
                            close(fd_in[1]);
                            dup2(fd_in[0], STDIN_FILENO);
                        }

                        else {
                            close(fd_in[1]);
                            close(fd_out[0]);
                            dup2(fd_in[0], STDIN_FILENO);
                            dup2(fd_out[1], STDOUT_FILENO);
                        }


                        char path[256];
                        char** args = (char**)calloc(256, sizeof(char*));
                        singlecommandparser(commandsin[k], path, args);

                        if (execvp(path, args) == -1) {
                            printf("ERROR\n");
                            exit(1);
                        }
                    exit(0);
                    } 
                    else {
                        close(fd_in[1]);
                        fd_in[0] = fd_out[0];
                        fd_in[1] = fd_out[1];

                        if (pipe(fd_out) != 0) {
                            printf("ERROR\n");
                            exit(1);
                        }

                    }


                }
            }
            while(wait(NULL)!=-1);
    }
    
    fclose(file);
}
