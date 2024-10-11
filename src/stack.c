#include "stack.h"

Stack *stack_init () {
    Stack *newStack = malloc(sizeof(Stack));
    if (newStack == NULL ) {
        return NULL;
    }
    newStack->top = -1;

    return newStack;
}

bool isEmpty(Stack *stack) {
    return stack->top == -1;
}

bool isFull(Stack *stack) {
    return stack->top == MAX_SIZE - 1;
}

int push(Stack *stack, int data) {
    if(isFull(stack)) {
        return -1; //ERROR STACK IS FULL
    }
    stack->arr[++stack->top] = data;
    return 0; // correctly pushed onto stack
}

int pop(Stack *stack) {
    if (isEmpty(stack)) {
        return -1; // ERROR STACK IS EMPTY
    }
    return stack->arr[stack->top--]; 
}

int peek (Stack *stack) {
    if (isEmpty(stack)) {
        return -1; // ERROR STACK IS EMPTY
    }

    return stack->arr[stack->top];
}

void stack_dstr(Stack *stack) {
    free(stack);
}