#ifndef STACK_H
#define STACK_H

#include "stdlibs.h"

/*start of typedef*/
typedef struct stack_node StackNode;
typedef struct stack Stack;
/*end of typedef*/

/*start of struct defs*/
typedef struct stack {
	StackNode *top;
	StackNode *head;
} Stack;
/*end of struct defs*/

/*start of func protos*/
Stack *new_stack();
void push(Stack *s, void *p);
void *pop(Stack *s);
void *top(const Stack *s);
void free_stack(Stack *s);
char is_stack_empty(const Stack *s);
/*end of func protos*/

#endif
