#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#include <dirent.h>

int *count;

int type_to_int(char* type){
    if (strcmp(type,"dir") == 0) return 0;
    if (strcmp(type,"block dev") == 0) return 1;
    if (strcmp(type,"char dev") == 0) return 2;
    if (strcmp(type,"fifo") == 0) return 3;
    if (strcmp(type,"file") == 0) return 4;
    if (strcmp(type,"slink") == 0) return 5;
    if (strcmp(type,"socket") == 0) return 6;
    else return 7;
}

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

void explore_dir(char *dir_name){
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
            printf("\n");
            char* r_path = realpath(path,NULL);
            printf("Real path: %s \n",r_path);
            free(r_path);
            printf("Number of links: %ld \n",stats.st_nlink);
            printf("Size (in bytes): %ld \n",stats.st_size);
            printf("Last time the file was accessed: %ld \n",stats.st_atime);
            printf("Last time the file was modified: %ld \n",stats.st_mtime);
            printf("Type: %s \n", type_of_file(stats));
            if (S_ISDIR(stats.st_mode)){
                explore_dir(path);
            }
            count[type_to_int(type_of_file(stats))]++;
        }

    }
    closedir(dir);



}

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Not enough arguments!!");
        return -1;
    }
    char dir_name[256];
    strcpy(dir_name, argv[1]);

    DIR *dir = opendir(dir_name);
    if (dir == NULL){
        printf("Can't open directory");
        return -1;
    }

    count = calloc(8,sizeof(int));
    explore_dir(dir_name);

    printf("\n\nDirs: %d  Block devs: %d  Char devs: %d  Fifos: %d  Regular files: %d  Symbolic links: %d  Sockets: %d \n\n",
           count[0],count[1],count[2],count[3],count[4],count[5],count[6]);

    free(count);
}