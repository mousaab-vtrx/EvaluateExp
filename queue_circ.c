#include "queue.h"
#include <stdlib.h>

struct Queue {
	Item *data;
	size_t head;
	size_t size;
	size_t capacity;
};

/* Recopier l'ordre FIFO au début du nouveau tableau. */
static DsStatus queue_grow(Queue *queue)
{
	Item *data;
	size_t index;

	data = malloc(queue->capacity * 2 * sizeof(*data));
	if (data == NULL) {
		return DS_NOMEM;
	}

	for (index = 0; index < queue->size; ++index) {
		data[index] = queue->data[(queue->head + index) % queue->capacity];
	}

	free(queue->data);
	queue->data = data;
	queue->head = 0;
	queue->capacity *= 2;
	return DS_OK;
}

Queue *queue_create(void)
{
	Queue *queue = calloc(1, sizeof(*queue));

	if (queue == NULL) {
		return NULL;
	}

	queue->capacity = 8;
	queue->data = malloc(queue->capacity * sizeof(*queue->data));
	if (queue->data == NULL) {
		free(queue);
		return NULL;
	}

	return queue;
}

void queue_destroy(Queue *queue)
{
	if (queue != NULL) {
		free(queue->data);
		free(queue);
	}
}

bool queue_is_empty(const Queue *queue)
{
	return queue == NULL || queue->size == 0;
}

size_t queue_size(const Queue *queue)
{
	return queue == NULL ? 0 : queue->size;
}

DsStatus queue_enqueue(Queue *queue, Item value)
{
	DsStatus status;

	if (queue == NULL) {
		return DS_BADARG;
	}

	if (queue->size == queue->capacity) {
		status = queue_grow(queue);
		if (status != DS_OK) {
			return status;
		}
	}

	queue->data[(queue->head + queue->size) % queue->capacity] = value;
	++queue->size;
	return DS_OK;
}

DsStatus queue_dequeue(Queue *queue, Item *out)
{
	if (queue == NULL || out == NULL) {
		return DS_BADARG;
	}

	if (queue->size == 0) {
		return DS_EMPTY;
	}

	*out = queue->data[queue->head];
	queue->head = (queue->head + 1) % queue->capacity;
	--queue->size;
	return DS_OK;
}

DsStatus queue_front(const Queue *queue, Item *out)
{
	if (queue == NULL || out == NULL) {
		return DS_BADARG;
	}

	if (queue->size == 0) {
		return DS_EMPTY;
	}

	*out = queue->data[queue->head];
	return DS_OK;
}
