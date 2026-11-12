#include "queue.h"
#include <stdlib.h>

typedef struct Node {
	struct Node *next;
	Item value;
} Node;

struct Queue {
	Node *head;
	Node *tail;
	size_t size;
};

Queue *queue_create(void)
{
	return calloc(1, sizeof(Queue));
}

void queue_destroy(Queue *queue)
{
	Node *next;

	if (queue == NULL) {
		return;
	}

	while (queue->head != NULL) {
		next = queue->head->next;
		free(queue->head);
		queue->head = next;
	}

	free(queue);
}

bool queue_is_empty(const Queue *queue)
{
	return queue == NULL || queue->head == NULL;
}

size_t queue_size(const Queue *queue)
{
	return queue == NULL ? 0 : queue->size;
}

DsStatus queue_enqueue(Queue *queue, Item value)
{
	Node *node;

	if (queue == NULL) {
		return DS_BADARG;
	}

	node = malloc(sizeof(*node));
	if (node == NULL) {
		return DS_NOMEM;
	}

	node->value = value;
	node->next = NULL;

	if (queue->tail == NULL) {
		queue->head = node;
	} else {
		queue->tail->next = node;
	}

	queue->tail = node;
	++queue->size;
	return DS_OK;
}

DsStatus queue_dequeue(Queue *queue, Item *out)
{
	Node *node;

	if (queue == NULL || out == NULL) {
		return DS_BADARG;
	}

	if (queue->head == NULL) {
		return DS_EMPTY;
	}

	node = queue->head;
	*out = node->value;
	queue->head = node->next;

	if (queue->head == NULL) {
		queue->tail = NULL;
	}

	free(node);
	--queue->size;
	return DS_OK;
}

DsStatus queue_front(const Queue *queue, Item *out)
{
	if (queue == NULL || out == NULL) {
		return DS_BADARG;
	}

	if (queue->head == NULL) {
		return DS_EMPTY;
	}

	*out = queue->head->value;
	return DS_OK;
}
