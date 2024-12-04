/**
 * linked_list_priv.h
 * Author: Marek Hric xhricma00
 * Private functions for linked list
 */

#include "linked_list.h"
#include <stdlib.h>
#include <string.h>

#ifndef LINKED_LIST_PRIV_H
#define LINKED_LIST_PRIV_H

// Private functions

// Check if the list is empty
bool LLIsEmpty(LinkedList_ptr list);

// Create a new node
LLNode_ptr LLCreateNode(char *key);

// Free a node
void LLFreeNode(LLNode_ptr node);

#endif // LINKED_LIST_PRIV_H
