#define NO_OPEN_FD 5
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#include <dirent.h>
#include <ftw.h>



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

char* type_of_file(const struct stat *stats){
    if (S_ISDIR(stats->st_mode)) return "dir";
    if (S_ISBLK(stats->st_mode)) return "block dev";
    if (S_ISCHR(stats->st_mode)) return "char dev";
    if (S_ISFIFO(stats->st_mode)) return "fifo";
    if (S_ISREG(stats->st_mode)) return "file";
    if (S_ISLNK(stats->st_mode)) return "slink";
    if (S_ISSOCK(stats->st_mode)) return "socket";
    return "different";
}

int explore(const char *path, const struct stat *stats, int flag, struct FTW *ftw) {
    printf("\n");
    char *r_path = realpath(path, NULL);
    printf("Real path: %s \n", r_path);
    free(r_path);
    printf("Number of links: %ld \n", stats->st_nlink);
    printf("Size (in bytes): %ld \n", stats->st_size);
    printf("Last time the file was accessed: %ld \n", stats->st_atime);
    printf("Last time the file was modified: %ld \n", stats->st_mtime);
    printf("Type: %s \n", type_of_file(stats));
    count[type_to_int(type_of_file(stats))]++;
    return 0;
}

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Not enough arguments!!");
        return -1;
    }
    char dir_name[256];
    strcpy(dir_name, argv[1]);
    count = calloc(8,sizeof(int));

    int flag = FTW_PHYS;
    if (nftw(dir_name, explore, NO_OPEN_FD, flag) == -1){
        printf("Error with nftw \n");
        return -1;
    }

    count[0] -= 1;

    printf("\n\nDirs: %d  Block devs: %d  Char devs: %d  Fifos: %d  Regular files: %d  Symbolic links: %d  Sockets: %d \n\n",
           count[0],count[1],count[2],count[3],count[4],count[5],count[6]);

    free(count);
}