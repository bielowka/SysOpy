#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/mman.h>

#define SHM_NAME "/kol_shm"
#define MAX_SIZE 1024

int main(int argc, char **argv)
{

    sleep(1);
    int val =0;
    /*******************************************
    Utworz/otworz posixowy obszar pamieci wspolnej "reprezentowany" przez SHM_NAME
    odczytaj zapisana tam wartosc i przypisz ja do zmiennej val
    posprzataj
    *********************************************/
    int mem = shm_open(SHM_NAME, O_CREAT | O_RDONLY, 0666);
    if (mem == -1){
        printf("errorororor\n");
    }
    ftruncate(mem,MAX_SIZE);
    void *place = mmap(NULL,MAX_SIZE,PROT_READ,MAP_SHARED,mem,0);
    val = atoi((char*)place);
    munmap(place,MAX_SIZE);
    shm_unlink(SHM_NAME);

	printf("%d square is: %d \n",val, val*val);
    return 0;
}
