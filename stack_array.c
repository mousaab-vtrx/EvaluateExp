#include "stack.h"
#include <stdlib.h>

struct Stack {
	Item *data;
	size_t size;
	size_t capacity;
};

Stack *stack_create(void)
{
	Stack *stack = calloc(1, sizeof(*stack));

	if (stack == NULL) {
		return NULL;
	}

	stack->capacity = 8;
	stack->data = malloc(stack->capacity * sizeof(*stack->data));
	if (stack->data == NULL) {
		free(stack);
		return NULL;
	}

	return stack;
}

void stack_destroy(Stack *stack)
{
	if (stack != NULL) {
		free(stack->data);
		free(stack);
	}
}

bool stack_is_empty(const Stack *stack)
{
	return stack == NULL || stack->size == 0;
}

size_t stack_size(const Stack *stack)
{
	return stack == NULL ? 0 : stack->size;
}

DsStatus stack_push(Stack *stack, Item value)
{
	Item *data;

	if (stack == NULL) {
		return DS_BADARG;
	}

	if (stack->size == stack->capacity) {
		data = realloc(stack->data, stack->capacity * 2 * sizeof(*data));
		if (data == NULL) {
			return DS_NOMEM;
		}

		stack->data = data;
		stack->capacity *= 2;
	}

	stack->data[stack->size] = value;
	++stack->size;
	return DS_OK;
}

DsStatus stack_pop(Stack *stack, Item *out)
{
	if (stack == NULL || out == NULL) {
		return DS_BADARG;
	}

	if (stack->size == 0) {
		return DS_EMPTY;
	}

	--stack->size;
	*out = stack->data[stack->size];
	return DS_OK;
}

DsStatus stack_top(const Stack *stack, Item *out)
{
	if (stack == NULL || out == NULL) {
		return DS_BADARG;
	}

	if (stack->size == 0) {
		return DS_EMPTY;
	}

	*out = stack->data[stack->size - 1];
	return DS_OK;
}
