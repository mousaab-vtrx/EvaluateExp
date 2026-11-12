#include "stack.h"
#include <stdlib.h>

typedef struct Node {
	struct Node *next;
	Item value;
} Node;

struct Stack {
	Node *top;
	size_t size;
};

Stack *stack_create(void)
{
	return calloc(1, sizeof(Stack));
}

void stack_destroy(Stack *stack)
{
	Node *next;

	if (stack == NULL) {
		return;
	}

	while (stack->top != NULL) {
		next = stack->top->next;
		free(stack->top);
		stack->top = next;
	}

	free(stack);
}

bool stack_is_empty(const Stack *stack)
{
	return stack == NULL || stack->top == NULL;
}

size_t stack_size(const Stack *stack)
{
	return stack == NULL ? 0 : stack->size;
}

DsStatus stack_push(Stack *stack, Item value)
{
	Node *node;

	if (stack == NULL) {
		return DS_BADARG;
	}

	node = malloc(sizeof(*node));
	if (node == NULL) {
		return DS_NOMEM;
	}

	node->value = value;
	node->next = stack->top;
	stack->top = node;
	++stack->size;
	return DS_OK;
}

DsStatus stack_pop(Stack *stack, Item *out)
{
	Node *node;

	if (stack == NULL || out == NULL) {
		return DS_BADARG;
	}

	if (stack->top == NULL) {
		return DS_EMPTY;
	}

	node = stack->top;
	*out = node->value;
	stack->top = node->next;
	free(node);
	--stack->size;
	return DS_OK;
}

DsStatus stack_top(const Stack *stack, Item *out)
{
	if (stack == NULL || out == NULL) {
		return DS_BADARG;
	}

	if (stack->top == NULL) {
		return DS_EMPTY;
	}

	*out = stack->top->value;
	return DS_OK;
}
