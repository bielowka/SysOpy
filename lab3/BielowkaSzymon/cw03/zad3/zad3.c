#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <wait.h>


#include <dirent.h>

int *count;

//int type_to_int(char* type){
//    if (strcmp(type,"dir") == 0) return 0;
//    if (strcmp(type,"block dev") == 0) return 1;
//    if (strcmp(type,"char dev") == 0) return 2;
//    if (strcmp(type,"fifo") == 0) return 3;
//    if (strcmp(type,"file") == 0) return 4;
//    if (strcmp(type,"slink") == 0) return 5;
//    if (strcmp(type,"socket") == 0) return 6;
//    else return 7;
//}

char* type_of_file(struct stat stats){
    if (S_ISDIR(stats.st_mode)) return "dir";
    if (S_ISBLK(stats.st_mode)) return "block dev";
    if (S_ISCHR(stats.st_mode)) return "char dev";
    if (S_ISFIFO(stats.st_mode)) return "fifo";
    if (S_ISREG(stats.st_mode)) return "file";
    if (S_ISLNK(stats.st_mode)) return "slink";
    if (S_ISSOCK(stats.st_mode)) return "socket";
    return "different";
}

int check(char *arg,struct stat stats, char *path,char *full_path){
    if (strcmp(type_of_file(stats),"file") != 0) return 0;
    if (strstr(path,".txt") == NULL) return 0;

    FILE *f = fopen(path,"r");
    if (!f){
        printf("Error with file \n");
        return 0;
    }
    char *line;
    size_t bufflen = 0;
    int lin;
    while ((lin = getline(&line,&bufflen,f)) != -1){
        if (strstr(line,arg) != NULL){
            printf("%s %d\n",full_path,getpid());
        }
    }
    free(line);
    fclose(f);
    return 1;
}

void explore_dir(char *dir_name,char *arg,int depth){
    if (depth <= 0) return;

    DIR *dir = opendir(dir_name);
    if (dir == NULL){
        return;
    }
    struct dirent *file;
    char path[256];
    while ((file = readdir(dir)) != NULL){
        strcpy(path,dir_name);
        strcat(path,"/");
        strcat(path,file->d_name);

        struct stat stats;
        if (stat(path,&stats) != 0){
            printf("Unable to get statistics from this file: %s \n",file->d_name);
        }
        else if (!(strcmp(file->d_name,".") == 0) && !(strcmp(file->d_name, "..") == 0)) {


            char full_path[256];
            strcpy(full_path,dir_name);
            strcat(full_path,"/");
            strcat(full_path,path);

            check(arg,stats, path,full_path);

            if (S_ISDIR(stats.st_mode)){
                pid_t a = fork();
                if (a == 0) {
                    explore_dir(path,arg,depth-1);
                    exit(0);
                }
            }
        }

    }
    closedir(dir);
}

int main(int argc, char *argv[]){
    if (argc != 4){
        printf("Wrong number of arguments!!");
        return -1;
    }
    char dir_name[256];
    strcpy(dir_name, argv[1]);

    DIR *dir = opendir(dir_name);
    if (dir == NULL){
        printf("Can't open directory");
        return -1;
    }


    explore_dir(dir_name,argv[2],atoi(argv[3]));

    while(wait(NULL) > 0);
    return 0;

}