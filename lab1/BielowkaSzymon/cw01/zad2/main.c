#include "labonelib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>

clock_t st_real, en_real;
struct tms st_cpu, en_cpu;
int nnum = 0;

double diff(clock_t t1, clock_t t2){
    return ((double)(t2 - t1) / sysconf(_SC_CLK_TCK));
}

void get_time(FILE *file)
{
    fprintf(file, "%15f\t%15f\t%15f\t\n",
            diff(st_real,en_real),
            diff(st_cpu.tms_utime,en_cpu.tms_utime),
            diff(st_cpu.tms_stime,en_cpu.tms_stime));
    printf("%15f\t%15f\t%15f\t\n",
            diff(st_real,en_real),
            diff(st_cpu.tms_utime,en_cpu.tms_utime),
            diff(st_cpu.tms_stime,en_cpu.tms_stime));
}


int main(int argc, char *argv[]) {
    FILE *raport;
    raport = fopen("raport2.txt","a");
    fprintf(raport, "%15s\t%15s\t%15s\n","Real","User","System");

    st_real = times(&st_cpu);

    int i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "create_table")==0) {
            if (i + 1 >= argc) {
                printf("Not enough arguments");
                return -1;
            }
            int size = atoi(argv[i+1]);
            create_table(size);
            i+=2;
        }
        else if (strcmp(argv[i], "remove_block")==0) {
            if (i + 1 >= argc) {
                printf("Not enough arguments");
                return -1;
            }
            int id = atoi(argv[i+1]);
            remove_block(id);
            i+=2;
        }
        else if (strcmp(argv[i], "wc_files")==0){
            if (i + 1 > argc) {
                printf("Not enough arguments");
                return -1;
            }
            int j = i + 1;
            while (j < argc){
                if (strcmp(argv[j],"wc_files") == 0 || strcmp(argv[j],"wc_files") == 0 || strcmp(argv[j],"remove_block" ) == 0 || strcmp(argv[j],"read") == 0 || strcmp(argv[j],"gettime") == 0 || strcmp(argv[j],"starttime") == 0 || strcmp(argv[j],"to_mem") == 0) break;
                j++;
            }
            if (j == i + 1) {
                printf("Not enough arguments");
                return -1;
            }
            int num = j - i - 1;
            nnum += num;
            char** filenames = (char**) calloc(num+1,sizeof(char*));
            for (int k = i + 1; k < j; k++) filenames[k-i-1] = argv[k];
            wc_files(num,filenames);
//            int res = tmpfile_to_mem(num);
//            printf("%d \n",res);
            i = j;
        }
        else if (strcmp(argv[i], "to_mem")==0){
            int res = tmpfile_to_mem(nnum);
            nnum = 0;
            i++;
        }
        else if (strcmp(argv[i], "read")==0){
            read_table();
            i++;
        }
        else if (strcmp(argv[i], "starttime")==0){
            st_real = times(&st_cpu);
            i++;
        }
        else if (strcmp(argv[i], "gettime")==0){
            en_real = times(&en_cpu);
            get_time(raport);
            i++;
        }
        else{
            printf("%s  Uknown command \n",argv[i]);
            return -1;
        }
    }
//    free_table();
    return 0;
}
