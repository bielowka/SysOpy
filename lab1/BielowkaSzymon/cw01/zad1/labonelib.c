#include "labonelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char*** Table;
int size;
int last_id;
int* block_size;

void create_table(int newsize){
    size = newsize;
    Table = (char***) calloc(size,sizeof(char**));
    last_id = 0;
    block_size = calloc(size, sizeof(int));
}

//void resize_table(int newsize){
//    if (newsize > Table.size){
//        block tmp;
//        tmp.val = Table.blocks.val;
//
//        for (int i = 0; i < Table.size){
//            remove_block(i);
//        }
//
//        Table.size = newsize;
//        Table.blocks = calloc(Table.size,sizeof(block));
//
//        Table.last_id = newsize-1;
//    Table.blocks = realloc(Table.blocks,sizeof(block) + sizeof(Table.blocks));
//    }
//}

void wc_files(int num, char** filenames){
    int size = 1;
    for (int i = 0; i < num; i++){
        size += sizeof(filenames[i]) + 1;
    }
    size += 12;
    char command[size];
    strcpy(command, "wc ");
    for (int i = 0; i < num; i++){
        strcat(command, filenames[i]);
        strcat(command, " ");
    }
    strcat(command,">>tmp.txt");
    strcat(command,"\0");
    system(command);
}

int tmpfile_to_mem(int num){
//    if (Table.last_id >= Table.size) resize_table(Table.size + 2);
    if (last_id >= size){
        printf("Error - not enought space in table");
        return -1;
    }

    FILE *f;
    f = fopen("tmp.txt","r");
    if (f == NULL){
        printf("Error - file does not exist");
        return -1;
    }

//    fseek(f, 0L, SEEK_END);
//    num = ftell(f);
//    fseek(f, 0L, SEEK_SET);

//    block Block = calloc(size,sizeof(char));
    char** Block = (char**) calloc(num+1,sizeof(char**));

    for (int i = 0; i < num; i++){
        char *buffer;
        size_t s = 100;
        buffer = (char *) calloc(100, sizeof(char));
        getline(&buffer,&s,f);
        Block[i] = buffer;
    }

    Table[last_id] = Block;
    block_size[last_id] = num + 1;
    last_id++;

    remove("tmp.txt");
//    printf("%d  ",size);
    return last_id-1;
}

void remove_row(int idb, int idr){
    if(Table[idb][idr] != NULL){
        free(Table[idb][idr]);
    }
}

void remove_block(int id){
    if(Table[id] == NULL){
        printf("error - %d is already empty \n",id);
        return;
    }

    if(id >= size){
        printf("error - index out of bound \n");
        return;
    }

    if(id >= last_id){
        printf("error - %d is already empty (over last_id %d) \n",id,last_id);
        return;
    }

    for (int i = 0; i<block_size[id]; i++){
        remove_row(id,i);
    }

    for (int i = id; i < last_id && i < size; i++){
        Table[i] = Table[i+1];
    }

//    for (int i = 0; i<block_size[last_id-1]; i++){
//        remove_row(last_id-1,i);
//    }


    free(Table[last_id-1]);
//    Table.size--;

    last_id--;
}




void free_table(){
    printf("Cleaning... \n");
    for (int i = 0; i < size; i++){
        remove_block(i);
    }
    free(Table);
}

void read_table(){
    printf("Beginning: \n");
    if (Table[0] == NULL){
        printf("Index empty");
        return;
    }
    for (int i = 0; i < last_id; i++){
        printf("%d:\n",i);
        for (int j = 0; j < block_size[i]; j++){
            if (Table[i][j] != NULL) printf("%s", Table[i][j]);
        }
        printf("\n");
    }
    printf("End \n");
}



