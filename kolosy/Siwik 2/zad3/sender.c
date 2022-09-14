#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>


#define KEY "./queuekey"

struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[1];    /* message data */
};


int main(int argc, char* argv[])
{
 
 if(argc !=2){
   printf("Not a suitable number of program parameters\n");
   return(1);
 }

 /******************************************************
 Utworz kolejke komunikatow systemu V "reprezentowana" przez KEY
 Wyslij do niej wartosc przekazana jako parametr wywolania programu 
 Obowiazuja funkcje systemu V
 ******************************************************/
    key_t key = ftok(KEY, 1);

    if (key == -1){
        printf("Error - key \n");
        return -1;
    }

    int server_queue = msgget(key, IPC_CREAT | 0666);

    if (server_queue == -1){
        printf("Error - server queue \n");
        return -1;
    }

    struct msgbuf buf;
    buf.mtype = 1;
    sprintf(buf.mtext,"%d", atoi(argv[1]));
    msgsnd(server_queue,&buf,sizeof(struct msgbuf),0);



  return 0;
}



