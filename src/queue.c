/*
 * Copyright (c) 2008 Sebastian Nowicki <sebnow@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "queue.h"
#include <stdlib.h>
#include <assert.h>
#include "testing_util.h"

queue_t *queueNew()
{
	queue_t *queue;
	queue = malloc(sizeof(*queue));
	if(queue != NULL) {
		queue->head = NULL;
		queue->tail = NULL;
		queue->count = 0;
	}
	return queue;
}

void queueInsert(queue_t *queue, void *data, queueNodeFreeFunction func)
{
	_queue_node_t *node;
	assert(queue != NULL);
	node = malloc(sizeof(*node));
	if(node == NULL) {
		return;
	}
	node->next = NULL;
	node->data = data;
	node->freeFunction = func;
	/* First node in queue */
	if(queue->tail == NULL) {
		queue->tail = node;
		queue->head = node;
	} else {
		queue->tail->next = node;
		queue->tail = node;
	}
	queue->count++;
}

int queueRemove(queue_t *queue, void **data)
{
	*data = NULL;
	_queue_node_t *node;
	if(queue->head == NULL) {
		return 0;
	}
	*data = queue->head->data;
	node = queue->head->next;
	free(queue->head);
	queue->head = node;
	queue->count--;
	if(queue->head == NULL) {
		queue->tail = NULL;
	}
	return 1;
}

size_t queueCount(queue_t *queue)
{
	return queue->count;
}

void queueFree(queue_t *queue)
{
	void *data = NULL;
	queueNodeFreeFunction freeFunction;
	if(queue->head != NULL) {
		freeFunction = queue->head->freeFunction;
		while(queueRemove(queue, &data)) {
			if(freeFunction != NULL && data != NULL) {
				freeFunction(data);
			}
			if(queue->head != NULL) {
				freeFunction = queue->head->freeFunction;
			}
		}
	}
	free(queue);
}