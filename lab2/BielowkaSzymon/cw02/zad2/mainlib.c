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
    raport = fopen("pomiar_zad_2.txt ","a");
    fprintf(raport, "%s\n", "Zad 2:");
    fprintf(raport, "%s\n", "Przy uzyciu funkcji bibliotecznych:");
    fprintf(raport, "%15s\t%15s\t%15s\n","Real","User","System");

    st_real = times(&st_cpu);

    FILE *we;
    char sign;
    char we_name[257];

    if (argc != 3){
        printf("Wrong number of arguments!!!");
        return -1;
    }
    else if (strlen(argv[1]) > 1){
        printf("Should be a single char!!!");
        return -1;
    }
    else {
        strcpy(we_name,argv[2]);
        strcpy(&sign,argv[1]);
    }

    we=fopen(we_name,"r");
    if (!we){
        printf("Something wrong with the file");
        return -1;
    }

    char c;
    int flag = 0;
    int counter = 0;
    int line_counter = 0;

    while(fread(&c,sizeof(char),1,we)>0){
        if (c == '\n'){
            if (flag == 1) {
                line_counter++;
            }
            flag = 0;
        }
        else {
            if (c == sign) {
                flag = 1;
                counter++;
            }
        }
    }

    printf("Result: %d %d\n",counter,line_counter);

    en_real = times(&en_cpu);
    get_time(raport);

    fclose(we);


}