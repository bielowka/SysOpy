#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#include <sys/times.h>
#include <time.h>
#include <sys/resource.h>


clock_t st_real, en_real;
struct tms st_cpu, en_cpu;

double diff(clock_t t1, clock_t t2){
    return ((double)(t2 - t1) / sysconf(_SC_CLK_TCK));
}

void get_time(FILE *file)
{
    fprintf(file, "%15f\t%15f\t%15f\t\n",
            diff(st_real,en_real),
            diff(st_cpu.tms_utime,en_cpu.tms_utime),
            diff(st_cpu.tms_stime,en_cpu.tms_stime));
}

int main(int argc, char *argv[])
{
    FILE *raport;
    raport = fopen("pomiar_zad_1.txt ","a");
    fprintf(raport, "%s\n", "Zad 1:");
    fprintf(raport, "%s\n", "Przy uzyciu funkcji bibliotecznych:");
    fprintf(raport, "%15s\t%15s\t%15s\n","Real","User","System");

    st_real = times(&st_cpu);

    FILE *we;
    FILE *wy;
    char we_name[256];
    char wy_name[256];
    if (argc == 1){
        scanf("%s",we_name);
        scanf("%s",wy_name);
    }
    else if (argc != 3){
        printf("Wrong number of arguments!!!");
        return -1;
    }
    else {
        strcpy(we_name,argv[1]);
        strcpy(wy_name,argv[2]);
    }



    we=fopen(we_name,"r");
    wy=fopen(wy_name,"w");
    if (!we || !wy){
        printf("Something wrong with the files");
        return -1;
    }

    char *c = (char *) calloc(1,sizeof(char));
    char *buffer = (char *) calloc(256,sizeof(char));
    int flag = 0;
    int endflag = 0;
    int i = 0;

    while(fread(c,sizeof(char),1,we)>0){
        if (c[0] == '\n'){
            endflag = 1;
            if (flag == 1) {
                buffer[i] = c[0];
                fwrite(buffer,sizeof(char),256,wy);
            }
            flag = 0;
            i = 0;
            free(buffer);
            buffer = (char *) calloc(256,sizeof(char));
        }
        else {
            endflag = 0;
            if (isspace(c[0]) == 0) {
                flag = 1;
            }
            buffer[i] = c[0];
            i++;
        }
        free(c);
        c = (char *) calloc(1,sizeof(char));
    }

    if (endflag == 0) fwrite(buffer,sizeof(char),256,wy);


    en_real = times(&en_cpu);
    get_time(raport);

    fclose(we);
    fclose(wy);

    free(buffer);
    free(c);

}