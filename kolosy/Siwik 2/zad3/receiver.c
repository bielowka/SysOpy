#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#define KEY  "./queuekey"

struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[1];    /* message data */
};

int main() {
        sleep(1);
        int val = 0;


	/**********************************
	Otworz kolejke systemu V "reprezentowana" przez KEY
	**********************************/
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


	


	while(1)
 	{	
	    /**********************************
	    Odczytaj zapisane w kolejce wartosci i przypisz je do zmiennej val
	    obowiazuja funkcje systemu V
	    ************************************/
        struct msgbuf buf;
        int res = msgrcv(server_queue,&buf,sizeof(struct msgbuf),-6,0);
        val = atoi(buf.mtext);
        printf("%d square is: %d\n", val, val*val);
        if (res > -1) break;
 
	}

	/*******************************
	posprzataj
	********************************/
    msgctl(server_queue, IPC_RMID,NULL);

     return 0;
   }
