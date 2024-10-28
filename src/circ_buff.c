/*
file: dyn_str.c
author: Marek Hric
Implementation of circular buffer functions from circ_buff.h.
*/

#include "../include/circ_buff.h"

#define INC_PTR(ptr) (ptr++) % CIRC_BUFF_SIZE

circ_buff_ptr circ_buff_init()
{
    circ_buff_ptr cb = (circ_buff_ptr)malloc(sizeof(circ_buff));
    if (cb == NULL) {
        fprintf(stderr, "Malloc failed\n");
        exit(1);
    }
    for (int i = 0; i < CIRC_BUFF_SIZE; i++) cb->buffer[i] = 0;
    cb->head = 0;
    cb->tail = 0;
    return cb;
}

void circ_buff_free(circ_buff_ptr cb)
{
    free(cb);
}

void circ_buff_enqueue(circ_buff_ptr cb, int c)
{
    cb->buffer[INC_PTR(cb->head)] = c;
}

int circ_buff_dequeue(circ_buff_ptr cb)
{
    return cb->buffer[INC_PTR(cb->tail)];
}

bool circ_buff_is_empty(circ_buff_ptr cb)
{
    return cb->head == cb->tail;
}
