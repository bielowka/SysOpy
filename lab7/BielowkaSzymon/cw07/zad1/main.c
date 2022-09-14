#include "utils.h"

int semaphores;
int oven_mem;
int table_mem;
Oven *oven;
Table *table;
//char* timer = calloc(21,sizeof(char));
//strftime(timer, 21, "%Y-%m-%d %H:%M:%S", localtime(&msg.time));
int *processes;
int num_processes;

void finito(int signum){
    printf("Pizzas left in oven: %d\n",oven->contents);
    printf("Pizzas left on table: %d\n",table->contents);
    printf("Oven semaphore: %d\n",semctl(semaphores, 1,GETVAL));
    printf("Table semaphore: %d\n",semctl(semaphores, 0,GETVAL));
    for (int i = 0; i <num_processes; i++){
        kill(processes[i],SIGINT);
    }

}

void single_finito(int signum){
    shmdt(oven);
    shmdt(table);
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
        struct sembuf buf;
        buf.sem_num = OVEN;
        buf.sem_op = -1;
        buf.sem_flg = 0;
        bool pizza_in = false;
        int where = -1;

        while (!pizza_in){
            buf.sem_op = -1;
            semop(semaphores,&buf,1);
            if (oven->storage[oven->next] == -1){
                oven->storage[oven->next] = pizzaType;
                where = oven->next;
                oven->next = (oven->next + 1) % oven_size;
                oven->contents++;
                pizza_in = true;
            }
            buf.sem_op = 1;
            semop(semaphores, &buf, 1);
        }
        printf("(%d %lu) Dodalem pizze: %d. Liczba pizz w piecu: %d.\n", getpid(), get_time(), pizzaType, oven->contents);
        sleep(4);
        pizza_in = false;
        where = -1;

        while (!pizza_in){
            buf.sem_op = -1;
            buf.sem_num = TABLE;
            semop(semaphores,&buf,1);
            if (table->storage[table->next] == -1){
                buf.sem_num = OVEN;
                semop(semaphores, &buf, 1);
                oven->storage[where] = -1;
                buf.sem_op = 1;
                oven->contents--;
                semop(semaphores, &buf, 1);
                table->storage[table->next] = pizzaType;
                table->next = (table->next + 1) % table_size;
                table->contents++;
                pizza_in = true;
            }
            buf.sem_num = TABLE;
            buf.sem_op = 1;
            semop(semaphores, &buf, 1);
        }
        printf("(%d %lu) Wyjmuje pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n",
               getpid(), get_time(), pizzaType, oven->contents, table->contents);
    }
}

void drive(){
    printf("Driver: Buon giorno!\n");
    signal(SIGINT, single_finito);
    while(1){
        struct sembuf buf;
        buf.sem_num = TABLE;
        buf.sem_op = -1;
        buf.sem_flg = 0;
        int pizzaType = -1;
        semop(semaphores, &buf, 1);
//        int which = (table->next - 1 + table_size) % table_size;
        int which = -1;
        for(int i = 0; i < table_size; i++){
            if(table->storage[i] != -1){
                which = i;
            }
        }

//        bool delivering = false;
        if (which != -1){
            pizzaType = table->storage[which];
            table->contents--;
            printf("(%d %lu) Pobieram pizze: %d. Liczba pizz na stole: %d.\n",getpid(), get_time(), table->storage[which], table->contents);
            table->storage[which] = -1;
//            delivering = true;
        }
        buf.sem_op = 1;
        semop(semaphores, &buf, 1);
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

    semaphores = new_semaphores();
    oven_mem = new_memory(sizeof(Oven),oven_key);
    table_mem = new_memory(sizeof(Table),table_key);

    oven = shmat(oven_mem, NULL, 0);
    table = shmat(table_mem, NULL,0);

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

    union semun opts;
    opts.val = 1;

    semctl(semaphores, OVEN, SETVAL, opts);
    semctl(semaphores, TABLE, SETVAL, opts);

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

    semctl(semaphores, 0, IPC_RMID);
    shmdt(oven);
    shmdt(table);
    shmctl(oven_mem, IPC_RMID, NULL);
    shmctl(table_mem, IPC_RMID, NULL);
    free(processes);
    exit(0);
}


