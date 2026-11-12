#include "stack.h"
#include <assert.h>

int main(void)
{
	Stack *stack;
	Item value;
	Item out;
	size_t index;

	stack = stack_create();
	assert(stack != NULL);
	assert(stack_pop(stack, &out) == DS_EMPTY);

	for (index = 0; index < 128; ++index) {
		value = (Item)index;
		assert(stack_push(stack, value) == DS_OK);
	}

	assert(stack_size(stack) == 128);

	for (index = 128; index > 0; --index) {
		assert(stack_pop(stack, &out) == DS_OK);
		assert(out == (Item)index - 1);
	}

	assert(stack_is_empty(stack));
	stack_destroy(stack);
	return 0;
}
