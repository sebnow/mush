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

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmockery.h>
#include "test_queue.h"
#include "queue.h"

void testQueueNew(void **state)
{
	queue_t *queue = queueNew();
	assert_true(queue->tail == NULL);
	assert_true(queue->head == NULL);
	queueFree(queue);
}

void testQueueInsert(void **state)
{
	queue_t *queue = queueNew();
	int data[3] = {1, 2, 3};
	void *headPtr;
	assert_true(queue != NULL);
	queueInsert(queue, &data[0], NULL);
	assert_true(queue->tail != NULL);
	assert_true(queue->head != NULL);
	assert_true(queue->tail == queue->head);
	assert_true(queue->head->data == &data[0]);
	headPtr = queue->head;
	queueInsert(queue, &data[1], NULL);
	/* Make sure head was not changed */
	assert_true(headPtr == queue->head);
	/* Data in head should also be the same */
	assert_true(queue->head->data == &data[0]);
	/* Check that data was added to the tail */
	assert_true(queue->tail->data == &data[1]);

	queueInsert(queue, &data[2], NULL);
	/* Head should still have the same data */
	assert_true(queue->head->data == &data[0]);
	/* Check that the nodes are connected properly */
	assert_true(queue->head->next->data == &data[1]);
	assert_true(queue->head->next->next == queue->tail);
	/* Check that data was added to the head */
	assert_true(queue->tail->data == &data[2]);

	queueFree(queue);
}

void testQueueRemove(void **state)
{
	queue_t *queue = queueNew();
	int data[3] = {1, 2, 3};
	int dataCount = 3;
	int index;
	int *ptr;

	queueInsert(queue, &data[0], NULL);
	queueRemove(queue, (void *)&ptr);
	assert_int_equal(*ptr, data[0]);

	for(index = 0; index < dataCount; index++) {
		queueInsert(queue, &data[index], NULL);
	}
	for(index = 0; index < dataCount; index++) {
		queueRemove(queue, (void *)&ptr);
		assert_int_equal(*ptr, data[index]);
	}
	queueFree(queue);
}

void testQueueCount(void **state)
{
	queue_t *queue = queueNew();

	int data = 1;
	assert_int_equal(queueCount(queue), 0);
	queueInsert(queue, &data, NULL);
	assert_int_equal(queueCount(queue), 1);
	queueInsert(queue, &data, NULL);
	assert_int_equal(queueCount(queue), 2);
	queueRemove(queue, (void *)&data);
	assert_int_equal(queueCount(queue), 1);
	queueRemove(queue, (void *)&data);
	assert_int_equal(queueCount(queue), 0);
	queueFree(queue);
}
