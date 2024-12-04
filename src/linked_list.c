/**
 * file: linked_list.c
 * author: Marek Hric xhricma00
 * Implementation of linked list
 * Used in code generator
 */

#include "linked_list_priv.h"


void LLInit(LinkedList_ptr list){
    list->head = NULL;
    list->last = NULL;
}

bool LLFind(LinkedList_ptr list, char *key)
{
    LLNode_ptr tmp = list->head;
    if (tmp == NULL)
        return false;
    
    while (tmp != NULL) {
        if (strcmp(tmp->key, key) == 0)
            return true;

        tmp = tmp->next;
    }

    return false;
}

int LLInsert(LinkedList_ptr list, char *key)
{
    LLNode_ptr node = LLCreateNode(key);
    if (node == NULL)
        return 1;

    if (LLIsEmpty(list)) {
        list->head = node;
        list->last = node;
    } else if (list->head == list->last) {
        list->head->next = node;
        list->last = node;
    } else {
        list->last->next = node;
        list->last = node;
    }

    return 0;
}

void LLDispose(LinkedList_ptr list)
{
    LLNode_ptr node = list->head;
    while (node != NULL)
    {
        LLNode_ptr tmp = node;
        node = node->next;
        LLFreeNode(tmp);
    }
    list->head = NULL;
    list->last = NULL;
}

bool LLIsEmpty(LinkedList_ptr list)
{
    if (list->head == NULL)
        return true;

    return false;
}

LLNode_ptr LLCreateNode(char *key)
{
    LLNode_ptr node = (LLNode_ptr)malloc(sizeof(LLNode_t));
    if (node == NULL)
        return NULL;

    node->key = malloc(strlen(key) + 1);
    if (node->key == NULL)
    {
        free(node);
        return NULL;
    }
    strcpy(node->key, key);
    node->next = NULL;

    return node;
}

void LLFreeNode(LLNode_ptr node)
{
    free(node->key);
    free(node);
}
