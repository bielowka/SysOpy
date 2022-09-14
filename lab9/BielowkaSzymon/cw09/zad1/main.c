#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>
#include <sys/file.h>
#include <time.h>
#include <sys/times.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>


int num_deliveries = 0;

pthread_t santa_thread;
pthread_cond_t santa_wakeup_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t santa_wakeup_mutex = PTHREAD_MUTEX_INITIALIZER;

int num_rein_wait;
pthread_mutex_t reindeers_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeers_wait_cond = PTHREAD_COND_INITIALIZER;
bool reindeer_waiting = false;
bool reindeers_waiting = false;
pthread_t * reindeers_threads;
pthread_mutex_t reindeers_go_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t * elves_threads;
int num_elves_problem = 0;
int num_elves_waiting = 0;
int elves_waiting[3];

pthread_mutex_t elves_wait_mutex;
pthread_cond_t elves_wait_cond;
pthread_mutex_t elves_go_mutex;


void* santa_action(void* arg){
    while(true){

        pthread_mutex_lock(&santa_wakeup_mutex);
        while (num_rein_wait < 9 && num_elves_waiting < 3){
            pthread_cond_wait(&santa_wakeup_cond, &santa_wakeup_mutex);
        }
        pthread_mutex_unlock(&santa_wakeup_mutex);

        printf("Komunikat: Mikołaj: budzę się\n");

        pthread_mutex_lock(&reindeers_go_mutex);
        if (num_rein_wait == 9) {

            num_deliveries++;
            printf("Komunikat: Mikołaj: dostarczam zabawki\n");
            sleep(rand()%3 + 2);
            num_rein_wait = 0;


            pthread_mutex_lock(&reindeers_wait_mutex);
            reindeers_waiting = false;
            pthread_cond_broadcast(&reindeers_wait_cond);
            pthread_mutex_unlock(&reindeers_wait_mutex);

            if (num_deliveries >= 3) exit(0);

        }
        pthread_mutex_unlock(&reindeers_go_mutex);


        pthread_mutex_lock(&elves_go_mutex);
        if (num_elves_waiting == 3) {
            printf("Komunikat: Mikołaj: rozwiązuje problemy elfów, %d %d %d\n",elves_waiting[0],elves_waiting[1],elves_waiting[2]);
            sleep(rand()%2 + 1);

            pthread_mutex_lock(&elves_wait_mutex);
            elves_waiting[0] = -1;
            elves_waiting[1] = -1;
            elves_waiting[2] = -1;
            num_elves_waiting = 0;
            pthread_cond_broadcast(&elves_wait_cond);
            pthread_mutex_unlock(&elves_wait_mutex);

        }
        pthread_mutex_unlock(&elves_go_mutex);


        printf("Komunikat: Mikołaj: zasypiam\n");
    }
}

void* reindeer_action(void *args){
    int ID = *((int *) args);
    while (true){
        sleep(rand()%6 + 5);

        pthread_mutex_lock(&reindeers_go_mutex);
        num_rein_wait++;
        reindeers_waiting = true;
        printf("Komunikat: Renifer: czeka %d reniferów na Mikołaja, %d\n",num_rein_wait,ID);
        if (num_rein_wait == 9){
            printf("Komunikat: Renifer: wybudzam Mikołaja, %d\n",ID);
            pthread_mutex_lock(&santa_wakeup_mutex);
            pthread_cond_broadcast(&santa_wakeup_cond);
            pthread_mutex_unlock(&santa_wakeup_mutex);
        }
        pthread_mutex_unlock(&reindeers_go_mutex);


        pthread_mutex_lock(&reindeers_wait_mutex);
        while (reindeers_waiting) {
            pthread_cond_wait(&reindeers_wait_cond, &reindeers_wait_mutex);
        }
        pthread_mutex_unlock(&reindeers_wait_mutex);
    }
}

void* elf_action(void *args){
    int ID = *((int *) args);
    while (true){
        sleep(rand()%4 + 2);

        pthread_mutex_lock(&elves_wait_mutex);
        while (num_elves_waiting == 3) {
            printf("Komunikat: Elf: czeka na powrót elfów, ID: %d\n", ID);
            pthread_cond_wait(&elves_wait_cond, &elves_wait_mutex);
        }
        pthread_mutex_unlock(&elves_wait_mutex);

        pthread_mutex_lock(&elves_go_mutex);

        if (num_elves_waiting < 3){
            elves_waiting[num_elves_waiting] = ID;
            num_elves_waiting++;
            printf("Komunikat: Elf: czeka %d elfów na Mikołaja, ID: %d\n", num_elves_waiting, ID);

            if (num_elves_waiting == 3){
                printf("Komunikat: Elf: wybudzam Mikołaja, ID: %d\n", ID);
                pthread_mutex_lock(&santa_wakeup_mutex);
                pthread_cond_broadcast(&santa_wakeup_cond);
                pthread_mutex_unlock(&santa_wakeup_mutex);
                printf("Komunikat: Elf: Mikołaj rozwiązuje problem, ID: %d\n", ID);
                sleep(rand()%2 + 1);
            }
        }
        pthread_mutex_unlock(&elves_go_mutex);

    }
}


int main(int argc, char *argv[]){
    srand(time(NULL));

    pthread_create(&santa_thread, NULL, &santa_action, NULL);

    int reindeers[9] = {0,0,0,0,0,0,0,0,0};
    reindeers_threads = calloc(9,sizeof(pthread_t));
    for (int i = 0; i < 9; i++){
        reindeers[i] = i;
        pthread_create(&reindeers_threads[i], NULL, &reindeer_action, &reindeers[i]);
    }

    int elves[10] = {0,0,0,0,0,0,0,0,0,0};
    elves_threads = calloc(10,sizeof(pthread_t));
    for (int i = 0; i < 10; i++){
        elves[i] = i;
        pthread_create(&elves_threads[i], NULL, &elf_action, &elves[i]);
    }

    pthread_join(santa_thread, NULL);

    for (int i = 0; i < 9; i++){
        pthread_join(reindeers_threads[i], NULL);
    }

    for (int i = 0; i < 10; i++){
        pthread_join(elves_threads[i], NULL);
    }

    free(reindeers_threads);
    free(elves_threads);
    return 0;



}