/*
file: circ_buff.h
author: Marek Hric
Header file for circular buffer.
*/

#ifndef CIRC_BUFF_H
#define CIRC_BUFF_H

#include <stdio.h>
#include <stdbool.h>

#define CIRC_BUFF_SIZE 8

typedef struct {
    int buffer[CIRC_BUFF_SIZE];
    int head;
    int tail;
} circ_buff, *circ_buff_ptr;

circ_buff_ptr circ_buff_init();
void circ_buff_free(circ_buff_ptr cb);
void circ_buff_enqueue(circ_buff_ptr cb, int c);
int circ_buff_dequeue(circ_buff_ptr cb);
bool circ_buff_is_empty(circ_buff_ptr cb);

#endif // CIRC_BUFF_H
