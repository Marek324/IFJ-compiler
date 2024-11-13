/*
file: stack.c
author: Adam Vesely
Implementation of the stack.
*/

#include <stdbool.h>
#include "common/stack.h"
#include "common/token.h"
#include "common/error.h"

stack_t* stackInit() {
    stack_t* stack = malloc(sizeof(stack_t));
    if(stack == NULL) {
        error_exit(99, "ERROR: Unable to allocate memory for stack!\n");
        return;
    }
    stack->top = NULL;
    return stack;
}

void stackPush(stack_t* stack, long data) {
    if(stack == NULL) {
        error_exit(99, "ERROR: Stack is not initialized!\n");
        return;
    }
    stack_elem* new_element = malloc(sizeof(stack_elem));
    if(new_element == NULL) {
        error_exit(99, "ERROR: Unable to allocate memory for stack element!\n");
        return;
    }
    new_element->data = data;
    new_element->next = stack->top;
    stack->top = new_element;
}

void stackPop(stack_t* stack) {
    if(stack == NULL) {
        error_exit(99, "ERROR: Stack is not initialized!\n");
        return;
    }
    if(stack->top != NULL) {
        stack_elem* element = stack->top;
        stack->top = stack->top->next;
        free(element);
    }
}

bool stackIsEmpty(stack_t* stack) {
    if(stack == NULL) {
        error_exit(99, "ERROR: Stack is not initialized!\n");
        return true;
    }
    return stack->top == NULL;
}

void stackClear(stack_t* stack) {
    if(stack != NULL) {
        while(!stackIsEmpty(stack)) {
            stackPop(stack);
        }
        free(stack);
    }
}

long stackGetTop(stack_t* stack) {
    if(stack == NULL) {
        error_exit(99, "ERROR: Stack is not initialized!\n");
        return;
    }
    return stack->top->data;
}