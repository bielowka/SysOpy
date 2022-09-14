#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <sys/file.h>
#include <time.h>
#include <sys/times.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#define WITH_NUMBERS 0
#define WITH_BLOCKS 1

int variant;
int width;
int height;

typedef struct{
    int id;
    int variant;
    int minVal;
    int maxVal;
    int from;
    int to;
    int **input;
    int **output;
    long time;
}Job;

void * thread_job(void * job){
    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);

    Job * todo = job;
    for (int i = 0; i < height; i++){
        for (int j = todo->from; j <= todo->to; j++){
            if (todo->input[i][j] >= todo->minVal && todo->input[i][j] <= todo->maxVal){
                todo->output[i][j] = 255 - todo->input[i][j];
            }
        }
    }
    clock_gettime(CLOCK_REALTIME, &end);

    long out = end.tv_sec*1e6+end.tv_nsec/1000 - begin.tv_sec*1e6-begin.tv_nsec/1000;
    todo->time=out;
    pthread_exit(&out);
}

int max(int a,int b){
    if (a > b) return a;
    return b;
}

int main(int argc, char *argv[]){

    if (argc != 5){
        printf("Error - wrong number of arguments!!!\n");
        exit(0);
    }

    int num_threads = atoi(argv[1]);
    char* variant_str = argv[2];
    char* input = argv[3];
    char* output = argv[4];

    if (num_threads < 1){
        printf("Error - number of threads cannot be less than 1!!!\n");
        exit(0);
    }

    if (strcmp(variant_str,"numbers")==0){
        variant = WITH_NUMBERS;
    }
    else if (strcmp(variant_str,"blocks")==0){
        variant = WITH_BLOCKS;
    }
    else {
        printf("Error - wrong variant!!!\n");
        exit(0);
    }

    FILE *input_file = fopen(input,"r");
    if (!input_file){
        printf("Error - cannot open input file!!!\n");
        exit(0);
    }

    FILE *output_file = fopen(output,"w+");
    if (!output_file){
        printf("Error - cannot open input file!!!\n");
        exit(0);
    }

    FILE *time_file = fopen("times.txt","a");
    fprintf(time_file,"%s;%d;",variant_str,num_threads);

    char* buffer = calloc(256,sizeof(char));
    size_t size = 256;
    getline(&buffer,&size,input_file);
    getline(&buffer,&size,input_file);
    getline(&buffer,&size,input_file);
    char* width_str = strtok(buffer," ");
    char* height_str = strtok(NULL,"\n");
    width = atoi(width_str);
    height = atoi(height_str);

    getline(&buffer,&size,input_file);


    int **image = calloc(height,sizeof(int*));
    for (int i = 0; i < height; i++){
        image[i] = calloc(width,sizeof(int));
    }

    int **negative = calloc(height,sizeof(int*));
    for (int i = 0; i < height; i++){
        negative[i] = calloc(width,sizeof(int));
    }

    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            int tmp;
            fscanf(input_file,"%d",&tmp);
            image[i][j] = tmp;
        }
    }

    pthread_t * threads = calloc(num_threads,sizeof(pthread_t));
    Job * jobs = calloc(num_threads,sizeof(Job));

    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);
    for (int i = 0; i < num_threads; i++){
        jobs[i].id = i;
        if (variant == WITH_NUMBERS){
            jobs[i].variant = WITH_NUMBERS;
            jobs[i].minVal = i*ceil(255/num_threads);
            jobs[i].maxVal = (i+1)*ceil(255/num_threads)-1;
            if (i == num_threads-1) jobs[i].maxVal = 255;
            jobs[i].from = 0;
            jobs[i].to = width;
            jobs[i].input = image;
            jobs[i].output = negative;
        }
        else {
            jobs[i].variant = WITH_BLOCKS;
            jobs[i].minVal = 0;
            jobs[i].maxVal = 256;
            jobs[i].from = i*ceil(width/num_threads);
            jobs[i].to = (i+1)*ceil(width/num_threads)-1;
            if (i == num_threads-1) jobs[i].to = width;
            jobs[i].input = image;
            jobs[i].output = negative;
        }
        pthread_create(&threads[i], NULL, thread_job, &jobs[i]);
    }


    for (int i = 0; i < num_threads; i++){
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < num_threads; i++){
        fprintf(time_file,"%ld ",jobs[i].time);
    }

    clock_gettime(CLOCK_REALTIME, &end);

    long time = end.tv_sec*1e6+end.tv_nsec/1000 - begin.tv_sec*1e6-begin.tv_nsec/1000;

    fprintf(time_file,";%ld;\n",time);

//    TEST
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if (negative[i][j] != 255-image[i][j]){
                printf("Error - not converted: %d %d :  %d   %d\n",i,j,image[i][j],negative[i][j]);
                exit(0);
            }
        }
    }

    fprintf(output_file, "P2\n%d %d\n255\n", width, height);
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(j != width-1)
                fprintf(output_file, "%d ", negative[i][j]);
            else
                fprintf(output_file, "%d\n", negative[i][j]);
        }
    }


    printf("%ld\n", time);

    free(buffer);
    free(image);
    free(negative);
    free(jobs);
    free(threads);

    fclose(input_file);
    fclose(output_file);
    fclose(time_file);
}