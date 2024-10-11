#ifndef STACK_H__
#define STACK_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>    

#define MAX_SIZE 5

typedef struct {
    // Array to store stack elements
    int arr[MAX_SIZE];  
    // Index of the top element in the stack
    int top;        
} Stack;

Stack *stack_init ();

bool isEmpty(Stack *stack);

bool isFull(Stack *stack);

int push(Stack *stack, int value);

int peek(Stack *stack);

int pop(Stack *stack);

void stack_dstr(Stack *stack);

#endif // STACK_H__