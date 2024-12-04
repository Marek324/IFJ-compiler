/**
 * file: c_buff.c
 * author: Marek Hric
 * Implementation of character buffer functions from c_buff.h.
 */

#include "c_buff.h"

#define INC_PTR(ptr) (ptr++) % C_BUFF_SIZE

c_buff_ptr c_buff_init()
{
    c_buff_ptr cb = (c_buff_ptr)malloc(sizeof(c_buff));
    if (cb == NULL) {
        fprintf(stderr, "Malloc failed\n");
        exit(1);
    }
    for (int i = 0; i < C_BUFF_SIZE; i++) cb->buffer[i] = 0;
    cb->head = 0;
    cb->tail = 0;
    return cb;
}

void c_buff_free(c_buff_ptr cb)
{
    free(cb);
}

void c_buff_enqueue(c_buff_ptr cb, int c)
{
    cb->buffer[INC_PTR(cb->head)] = c;
}

int c_buff_dequeue(c_buff_ptr cb)
{
    return cb->buffer[INC_PTR(cb->tail)];
}

bool c_buff_is_empty(c_buff_ptr cb)
{
    return cb->head == cb->tail;
}
