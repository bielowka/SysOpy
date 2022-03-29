//
// Created by sbiel on 10.03.2022.
//

#ifndef LABONELIB_C_LABONELIB_H
#define LABONELIB_C_LABONELIB_H

//typedef struct block{
//    char** val;
//    int size;
//} block;
//
//typedef struct table{
//    block* blocks;
//    int size;
//    int last_id;
//} table;

void create_table(int size);

void resize_table(int newsize);

void wc_files(int num, char** filenames);

int to_mem(int num);

void remove_row(int idb, int idr);

void remove_block(int id);

void free_table();

void read_table();









#endif //LABONELIB_C_LABONELIB_H
