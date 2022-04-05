#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <string.h>
#include <wait.h>

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
    printf("%15f\t%15f\t%15f\t\n",
            diff(st_real,en_real),
            diff(st_cpu.tms_utime,en_cpu.tms_utime),
            diff(st_cpu.tms_stime,en_cpu.tms_stime));
}

double func(double x) {
    return 4/(x * x + 1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Wrong number of arguments");
        return -1;
    }

    double dx = atof(argv[1]);
    int f = atoi(argv[2]);
    int n = 0;
    double last;
    int per_process = 0;
    int for_last_process = 0;

    int tmp = (int) (1 / dx);

    if (tmp*dx == 1){
        n = 1 / atof(argv[1]);
        last = dx;
    }
    else{
        n = (1 / atof(argv[1])) + 1;
        last = 1.0 - ((double)(n-1)*dx);
    }

    if (f > n){
        printf("More processes than squares");
        return -1;
    }

    if (n%f == 0){
        per_process = n / f;
        for_last_process = n / f;
    }
    else{
        per_process = n / f;
        for_last_process = n/f + n - (per_process*f);
    }

    st_real = times(&st_cpu);
    for (int i = 0; i < f; i++) {
        pid_t a = fork();
        if (a == 0){
            int curr_per_process = per_process;
            float curr_dx = dx;
            if (i == f - 1){
                curr_per_process = for_last_process;
            }
//            printf("I am the process %d with number of squares %d\n",i,per_process);
            double sum = 0;
            for (int j = 0; j < curr_per_process; j++){
                if (i == f - 1){
                    if (j == curr_per_process-1){
                        curr_dx = last;
                    }
                }
                double beg = i * per_process * dx + j * dx;
                double end = i * per_process * dx + j * dx + curr_dx;
                double x = (end + beg) / 2;

                sum += func(x)*curr_dx;


            }
            FILE *tmp;
            char file_name[7];
            sprintf(file_name, "w%d.txt", i+1);
            tmp = fopen(file_name,"w");
            fprintf(tmp,"%.15lf",sum);
            fclose(tmp);
            exit(0);
        }
        else{
            continue;
        }
    }

    while(wait(NULL) > 0);

    FILE *raport;
    raport = fopen("raportzad2.txt","a");
    fprintf(raport, "DX: %.16f  Processes: %d \n",dx,f);
    fprintf(raport, "%15s\t%15s\t%15s\n","Real","User","System");
    en_real = times(&en_cpu);
    get_time(raport);
    fprintf(raport, "\n\n");


    double res = 0.0;
    for (int i = 0; i<f; i++){
        FILE *tmp;
        char file_name[7];
        sprintf(file_name, "w%d.txt", i+1);
        tmp = fopen(file_name,"r");
        double tmp_val;
        fscanf(tmp,"%lf",&tmp_val);
        fclose(tmp);
        res += tmp_val;
    }




    printf("%lf",res);


}