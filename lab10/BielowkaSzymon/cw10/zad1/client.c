#include "utils.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int server_socket;
char buffer[MSG_LEN + 1];
char *nick, *command, *arg;
char my_symbol;
char rival_symbol;

int game_state = INIT;
char board[9] = "012345678";

int is_winner(){
    int winner = -1;
    if (board[0] == 'o' && board[1] == 'o' && board[2] == 'o') winner = 0;
    if (board[3] == 'o' && board[4] == 'o' && board[5] == 'o') winner = 0;
    if (board[6] == 'o' && board[7] == 'o' && board[8] == 'o') winner = 0;

    if (board[0] == 'o' && board[3] == 'o' && board[6] == 'o') winner = 0;
    if (board[1] == 'o' && board[4] == 'o' && board[7] == 'o') winner = 0;
    if (board[2] == 'o' && board[5] == 'o' && board[8] == 'o') winner = 0;

    if (board[0] == 'o' && board[4] == 'o' && board[8] == 'o') winner = 0;
    if (board[2] == 'o' && board[4] == 'o' && board[6] == 'o') winner = 0;

    if (board[0] == 'X' && board[1] == 'X' && board[2] == 'X') winner = 1;
    if (board[3] == 'X' && board[4] == 'X' && board[5] == 'X') winner = 1;
    if (board[6] == 'X' && board[7] == 'X' && board[8] == 'X') winner = 1;

    if (board[0] == 'X' && board[3] == 'X' && board[6] == 'X') winner = 1;
    if (board[1] == 'X' && board[4] == 'X' && board[7] == 'X') winner = 1;
    if (board[2] == 'X' && board[5] == 'X' && board[8] == 'X') winner = 1;

    if (board[0] == 'X' && board[4] == 'X' && board[8] == 'X') winner = 1;
    if (board[2] == 'X' && board[4] == 'X' && board[6] == 'X') winner = 1;

    return winner;
}

void* play(void* args){
    while(true){
        if (game_state == INIT){
            if (strcmp(arg, "name_taken") == 0){
                printf("This name is already taken!!!\n");
                exit(1);
            }
            else if (strcmp(arg, "no_rival") == 0) {
                printf("Wait for your rival\n");
                game_state = WAIT_RIVAL;
            }
            else{
                printf("You are: (%c)\n",arg[0]);
                bool is_tick = arg[0] == 'X';
                game_state = is_tick ? PLAY : WAIT_MOVE;
                my_symbol = is_tick ? 'X' : 'o';
                rival_symbol = is_tick ? 'o' : 'X';
            }
        }
        else if (game_state == WAIT_RIVAL){

            pthread_mutex_lock(&mutex);
            while (game_state != INIT && game_state != END) pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);

            bool is_tick = arg[0] == 'X';
            game_state = is_tick ? PLAY : WAIT_MOVE;
            my_symbol = is_tick ? 'X' : 'o';
            rival_symbol = is_tick ? 'o' : 'X';
        }
        else if (game_state == WAIT_MOVE){
            pthread_mutex_lock(&mutex);
            while (game_state != RIVAL_PLAY && game_state != END) pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
        else if (game_state == RIVAL_PLAY){
            int pos = atoi(arg);
            if (pos < 0 || pos > 9 || board[pos] == 'X' || board[pos] == 'o'){
                printf("Wrong position!!!\n");
            }
            board[pos] = rival_symbol;

            bool is_draw = true;
            for (int i = 0; i < 9; i ++){
                if (board[i] != 'o' && board[i] != 'X'){
                    is_draw = false;
                    break;
                }
            }

            int winner = is_winner();

            if (is_draw){
                printf("Draw!!!\n");
                game_state = END;
            }
            else if (winner != -1){
                if (winner == 0 && my_symbol == 'o'){
                    printf("You won!!!\n");
                }
                else if (winner == 1 && my_symbol == 'X'){
                    printf("You won!!!\n");
                }
                else printf("You lost!!!\n");
                game_state = END;
            }

            if (game_state != END){
                game_state = PLAY;
            }
        }
        else if (game_state == PLAY){
            printf("-----------\n");
            printf(" %c | %c | %c \n %c | %c | %c \n %c | %c | %c \n",
                   board[0],board[1],board[2],board[3],board[4],board[5],board[6],board[7],board[8]);
            printf("-----------\n");
            int pos;
            do{
                printf("Make a move (%c): ", my_symbol);
                scanf("%d", &pos);
            } while (pos < 0 || pos > 9 || board[pos] == 'X' || board[pos] == 'o');

            board[pos] = my_symbol;
            printf("---------\n");
            printf(" %c | %c | %c \n %c | %c | %c \n %c | %c | %c \n",
                   board[0],board[1],board[2],board[3],board[4],board[5],board[6],board[7],board[8]);
            printf("---------\n");
            char buffer[MSG_LEN + 1];
            sprintf(buffer, "move:%d:%s", pos, nick);
            send(server_socket, buffer, MSG_LEN, 0);

            bool is_draw = true;
            for (int i = 0; i < 9; i ++){
                if (board[i] != 'o' && board[i] != 'X'){
                    is_draw = false;
                    break;
                }
            }

            int winner = is_winner();

            if (is_draw){
                printf("Draw!!!\n");
                game_state = END;
            }
            else if (winner != -1){
                if (winner == 0 && my_symbol == 'o'){
                    printf("You won!!!\n");
                }
                else if (winner == 1 && my_symbol == 'X'){
                    printf("You won!!!\n");
                }
                else printf("You lost!!!\n");
                game_state = END;
            }

            if (game_state != END){
                game_state = WAIT_MOVE;
            }
        }
        else if (game_state == END) {
            char buffer[MSG_LEN + 1];
            sprintf(buffer, "end: :%s", nick);
            send(server_socket, buffer, MSG_LEN, 0);
            exit(0);
        }
    }
}

void end(){
    char buffer[MSG_LEN + 1];
    sprintf(buffer, "end: :%s", nick);
    send(server_socket, buffer, MSG_LEN, 0);
    exit(0);
}

int main(int argc, char *argv[]){

    if (argc != 4){
        printf("Wrong number of arguments!!!\n");
        exit(0);
    }

    signal(SIGINT, end);
    nick = argv[1];

    if (strcmp(argv[2], "unix") == 0){
        char* path = argv[3];
        server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
        struct sockaddr_un sa;
        memset(&sa, 0, sizeof(struct sockaddr_un));
        sa.sun_family = AF_UNIX;
        strcpy(sa.sun_path, path);

        if (connect(server_socket, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
            printf("Cannot connect server socket!!!\n");
            exit(1);
        }
    }
    else if (strcmp(argv[2], "inet") == 0){
        struct addrinfo *info;
        struct addrinfo hints;
        char *port = argv[3];
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        int a = getaddrinfo("localhost",port, &hints, &info);
        if (a != 0) {
            printf("(%s)\n", gai_strerror(a));
            exit(0);
        }
        server_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

        if (server_socket  == -1){
            printf("Cannot create server socket!!!\n");
            exit(1);
        }
        if (connect(server_socket, info->ai_addr, info->ai_addrlen) < 0){
            printf("Cannot connect server socket!!!\n");
            exit(1);
        }

        freeaddrinfo(info);

    }
    else{
        printf("Wrong method!!!\n");
        exit(1);
    }

    char msg[MSG_LEN];
    sprintf(msg, "add: :%s", nick);
    send(server_socket, msg, MSG_LEN, 0);

    bool is_running = false;

    while(true){
        recv(server_socket, buffer, MSG_LEN, 0);
        command = strtok(buffer, ":");
        arg = strtok(NULL, ":");

        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0)
        {
            game_state = INIT;
            if (!is_running)
            {
                pthread_t game;
                pthread_create(&game, NULL,&play, NULL);
                is_running = true;
            }
        }
        else if (strcmp(command, "move") == 0)
        {
            game_state = RIVAL_PLAY;
        }
        else if (strcmp(command, "end") == 0)
        {
            game_state = END;
            exit(0);
        }
        else if (strcmp(command, "ping") == 0)
        {
            sprintf(buffer, "reply: :%s", nick);
            send(server_socket, buffer, MSG_LEN, 0);
        }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);

    }






}
