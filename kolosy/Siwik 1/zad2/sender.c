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
    
   if(argc !=2){
    printf("Not a suitable number of program parameters\n");
    return(1);
   }

   /***************************************
   Utworz/otworz posixowy obszar pamieci wspolnej "reprezentowany" przez SHM_NAME
   zapisz tam wartosc przekazana jako parametr wywolania programu
   posprzataj
   *****************************************/
    int mem = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (mem == -1){
        printf("errorororor\n");
    }
    ftruncate(mem,MAX_SIZE);
    void *place = mmap(NULL,MAX_SIZE,PROT_READ | PROT_WRITE,MAP_SHARED,mem,0);
    int val = atoi(argv[1]);
    sprintf(place, "%d", val);
 
    return 0;
}
