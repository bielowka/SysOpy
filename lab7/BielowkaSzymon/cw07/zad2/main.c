#include "utils.h"

sem_t *oven_sem;
sem_t *table_sem;
int oven_mem;
int table_mem;
Oven *oven;
Table *table;
int *processes;
int num_processes;

void finito(int signum){
    printf("Pizzas left in oven: %d\n",oven->contents);
    printf("Pizzas left on table: %d\n",table->contents);
    for (int i = 0; i <num_processes; i++){
        kill(processes[i],SIGINT);
    }

}

void single_finito(int signum){
    munmap(oven, oven_size);
    munmap(table, table_size);
    sem_close(oven_sem);
    sem_close(table_sem);
    exit(0);
}

void cook(){
    printf("Cook: Buon giorno!\n");
    srand(get_time() + getpid());
    signal(SIGINT, single_finito);


    while(1){
        int pizzaType = rand() % 10;
        sleep(1);
        printf("(%d %lu) Przygotowuje pizze: %d\n", getpid(), get_time(), pizzaType);
        bool pizza_in = false;
        int where = -1;

        while (!pizza_in){
            sem_wait(oven_sem);
            if (oven->storage[oven->next] == -1){
                oven->storage[oven->next] = pizzaType;
                where = oven->next;
                oven->next = (oven->next + 1) % oven_size;
                oven->contents++;
                pizza_in = true;
            }
            sem_post(oven_sem);
        }
        printf("(%d %lu) Dodalem pizze: %d. Liczba pizz w piecu: %d.\n", getpid(), get_time(), pizzaType, oven->contents);
        sleep(4);
        pizza_in = false;
        where = -1;

        while (!pizza_in){
            sem_wait(table_sem);
            if (table->storage[table->next] == -1){
                sem_wait(oven_sem);
                oven->storage[where] = -1;
                oven->contents--;
                sem_post(oven_sem);
                table->storage[table->next] = pizzaType;
                table->next = (table->next + 1) % table_size;
                table->contents++;
                pizza_in = true;
            }
            sem_post(table_sem);
        }
        printf("(%d %lu) Wyjmuje pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n",
               getpid(), get_time(), pizzaType, oven->contents, table->contents);
    }
}

void drive(){
    printf("Driver: Buon giorno!\n");
    signal(SIGINT, single_finito);
    while(1){
        int pizzaType = -1;
        sem_wait(table_sem);
        int which = -1;
        for(int i = 0; i < table_size; i++){
            if(table->storage[i] != -1){
                which = i;
            }
        }

        if (which != -1){
            pizzaType = table->storage[which];
            table->contents--;
            printf("(%d %lu) Pobieram pizze: %d. Liczba pizz na stole: %d.\n",getpid(), get_time(), table->storage[which], table->contents);
            table->storage[which] = -1;

        }
        sem_post(table_sem);
        if(which != -1){
            sleep(4);
            printf("(%d %lu) Dostarczam pizze: %d.\n", getpid(), get_time(), pizzaType);
            sleep(4);
        }
    }
}


int main(int argc, char* argv[]) {

    if (argc != 3){
        printf("Error - wrong number of arguments!!!\n");
        exit(0);
    }

    int num_cooks = atoi(argv[1]);
    if (num_cooks < 1){
        printf("Error - wrong number of cooks!!!\n");
        exit(0);
    }

    int num_drivers = atoi(argv[2]);
    if (num_drivers < 1){
        printf("Error - wrong number of drivers!!!\n");
        exit(0);
    }

    signal(SIGINT, finito);
    num_processes = num_drivers+num_cooks;
    processes = calloc(num_processes,sizeof(int));
    int index = 0;

    oven_sem = new_semaphores(oven_sem_name);
    table_sem = new_semaphores(table_mem_name);

    oven_mem = new_memory(oven_mem_name);
    table_mem = new_memory(table_mem_name);

    if(oven_mem != -1){
        ftruncate(oven_mem, sizeof(Oven));
    }
    else{
        printf("Error - shared memory\n");
        exit(0);
    }

    if(table_mem != -1){
        ftruncate(table_mem, sizeof(Table));
    }
    else{
        printf("Error - shared memory\n");
        exit(0);
    }
    oven = mmap(NULL, oven_size, PROT_READ | PROT_WRITE, MAP_SHARED, oven_mem, 0);
    table = mmap(NULL, table_size, PROT_READ | PROT_WRITE, MAP_SHARED, table_mem, 0);

    oven->contents = 0;
    table->contents = 0;

    oven->next = 0;
    table->next = 0;

    for(int i = 0; i < oven_size; i++){
        oven->storage[i] = -1;
    }

    for(int i = 0; i < table_size; i++){
        table->storage[i] = -1;
    }


    pid_t a = 0;
    for(int i = 0; i < num_cooks; i++) {
        a = fork();
        if (a == 0){
            cook();
            exit(0);
        }
        else{
            processes[index++]=a;
        }
    }

    for(int i = 0; i < num_drivers; i++) {
        a = fork();
        if (a == 0){
            drive();
            exit(0);
        }
        else{
            processes[index++]=a;
        }
    }



    while(wait(NULL) > 0);

    printf("Finito! \n");

    munmap(oven, oven_size);
    munmap(table, table_size);
    shm_unlink(oven_mem_name);
    shm_unlink(table_mem_name);
    sem_close(oven_sem);
    sem_close(table_sem);
    sem_unlink(oven_sem_name);
    sem_unlink(table_sem_name);
    free(processes);
    exit(0);
}


