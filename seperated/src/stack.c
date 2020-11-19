#include "../include/stack.h"

/*start of struct defs*/
typedef struct stack_node{
	void *payload;
	StackNode *next;
} StackNode;
/*end of struct defs*/

/*start of func defs*/
Stack *new_stack(){
	Stack *s = (Stack*)malloc(sizeof(Stack));
	s->head = NULL;
	s->top = NULL;
	return s;
}
void push(Stack *s, void *p){
	StackNode *n = (StackNode*)malloc(sizeof(StackNode));
	n->payload = p;

	if (s->head == NULL) { // empty stack
		s->top = n;
		s->head = n;
	}else{
		s->top->next = n;
		s->top = s->top->next;
	}
}
void *top(const Stack *s){
	return (s->top == NULL)? NULL : s->top->payload;
}
void *pop(Stack *stack){
	if(stack->head == NULL)
		return NULL;
	StackNode *sn = stack->head;
	if(sn == stack->top){ // contains a single stacknode
		void *tmp = sn->payload;
		free(sn);
		stack->top = NULL;
		stack->head = NULL;
		return tmp;
	}
	// find stack node under top
	while(sn->next != stack->top)
		sn = sn->next;
	void *tmp = stack->top->payload;
	free(stack->top);
	sn->next = NULL;
	stack->top = sn;
	return tmp;
}
char is_stack_empty(const Stack *s){
	return ((s == NULL) || (s->head == NULL));
}
void free_stack(Stack *s){
	while ( !is_stack_empty(s) )
		pop(s);
	free(s);
}
/*end of func defs*/