/**
 * linked_list.h
 * Author: Marek Hric xhricma00
 * Public functions for linked list
 */

#include <stdlib.h>
#include <stdbool.h>

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct LLNode_s{
    char *key;
    struct LLNode_s *next;
} LLNode_t, *LLNode_ptr;

typedef struct LinkedList_s{
    LLNode_ptr head;
    LLNode_ptr active;
    LLNode_ptr last;
} LinkedList_t, *LinkedList_ptr;

// Public functions

// Initialize the linked list
void LLInit(LinkedList_ptr list);

/**
 * Insert a new key at the end of the list
 * return 0 if successful,
 * 1 otherwise
 */ 
int LLInsert(LinkedList_ptr list, char *key);

/**
 * Find a key in the list
 * return true if the key is found, 
 * false otherwise
 */
bool LLFind(LinkedList_ptr list, char *key);

// Free entire list
void LLDispose(LinkedList_ptr list);

#endif // LINKED_LIST_H
