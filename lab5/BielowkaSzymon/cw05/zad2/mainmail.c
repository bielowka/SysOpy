#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>

/**
 * nie udalo mi sie wyslac czegokolwiek do skrzynki w linuxie wiec nie bylem w stanie w pelni sprawdzic poprawnosci programu z uzyciem mail
 * w drugiej wersji (main.c) maile sa zapisywane do pliku mbox znajdujacego sie w tym samym folderze
 * ta kompiluje sie i nie zwraca bledow ale nie bylem w stanie zweryfikowac szczegolow
 */

void reademails(char *orderby){
    printf("Reading ordered by: %s \n",orderby);
    char* command;
    FILE *fd;
    if (strcmp(orderby,"data") == 0) {
//        command = "echo | mail";
        command = "mail";
    }
    else if (strcmp(orderby, "nadawca") == 0){
//        command = "echo | mail | sort -k 3";
        command = "mail | sort -k 3";
    }

    fd = popen(command,"r");
    char *line;
    size_t len = 256;
    while(getline(&line, &len, fd) != -1){
        printf("%s\n", line);
    }
    pclose(fd);
}

void sendemail(char* receiver, char* topic, char* content){
    printf("Sending message: %s  with topic: %s  to %s\n",content,topic,receiver);
    char *command = calloc(256,sizeof(char));
    sprintf(command,"echo %s | mail -s %s %s", content, topic, receiver);
    FILE* fd = popen(command,"w");
    printf("Sent \n");
    free(command);
    pclose(fd);
}

int main(int argc, char* argv[]){
    if (argc == 2){
        reademails(argv[1]);
    }
    else if (argc == 4){
        char* receiver = argv[1];
        char* topic = argv[2];
        char* content = argv[3];

        sendemail(receiver,topic,content);

    }
    else {
        printf("Wrong number of arguments!!!\n");
    }
}



