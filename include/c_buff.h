/**
 * file: c_buff.h
 * author: Marek Hric xhricma00
 * Header file for character buffer
 * Used in scanner
 */

#ifndef C_BUFF_H
#define C_BUFF_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define C_BUFF_SIZE 8

typedef struct {
    int buffer[C_BUFF_SIZE];
    int head;
    int tail;
} c_buff, *c_buff_ptr;

c_buff_ptr c_buff_init();
void c_buff_free(c_buff_ptr cb);
void c_buff_enqueue(c_buff_ptr cb, int c);
int c_buff_dequeue(c_buff_ptr cb);
bool c_buff_is_empty(c_buff_ptr cb);

#endif // C_BUFF_H
