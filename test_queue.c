#include "queue.h"
#include <assert.h>

int main(void)
{
	Queue *queue;
	Item value;
	Item out;
	size_t index;
	size_t removed = 0;

	queue = queue_create();
	assert(queue != NULL);
	assert(queue_dequeue(queue, &out) == DS_EMPTY);

	for (index = 0; index < 128; ++index) {
		value = (Item)index;
		assert(queue_enqueue(queue, value) == DS_OK);

		if (index % 3 == 2) {
			assert(queue_dequeue(queue, &out) == DS_OK);
			assert(out == (Item)removed);
			++removed;
		}
	}

	while (!queue_is_empty(queue)) {
		assert(queue_dequeue(queue, &out) == DS_OK);
	}

	value = 42;
	assert(queue_enqueue(queue, value) == DS_OK);
	assert(queue_dequeue(queue, &out) == DS_OK);
	assert(out == 42);

	queue_destroy(queue);
	return 0;
}
