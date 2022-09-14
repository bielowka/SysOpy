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
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netdb.h>
#include <poll.h>
#include <string.h>
#include <signal.h>
#include <limits.h>

#define MAX_CLIENTS 16
#define PING 10
#define MSG_LEN 512
#define SOCKET_EVENT 0
#define CLIENT_EVENT 1

#define INIT 0
#define WAIT_RIVAL 1
#define WAIT_MOVE 2
#define PLAY 3
#define RIVAL_PLAY 4
#define END 5


typedef struct client{
    int fd;
    int rival;
    char *nick;
    char symbol;
    bool active;
} client;

struct event {
    int type;
    client* client;
    int socket;
};