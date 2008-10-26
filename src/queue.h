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

#ifndef QUEUE_H
#define QUEUE_H

#include <unistd.h>

/*!
 \addtogroup queue
 \{
 */

/*! \brief Prototype for the function callback used to free queue node data */
typedef void (*queueNodeFreeFunction)(void *);

typedef struct __queue_node_t {
	/*! \brief The data stored in the node */
	void *data;
	/*! \brief Next node in the queue */
	struct __queue_node_t *next;
	/*! \brief Callback used for freeing \a data */
	queueNodeFreeFunction freeFunction;
} _queue_node_t;

/*!
 \brief FIFO queue structure
 */
typedef struct __queue_t {
	/*! \brief Head (oldest) node of the queue */
	struct __queue_node_t *head;
	/*! \brief Tail (newest) node of the queue */
	struct __queue_node_t *tail;
	/*! \brief Amount of nodes in the queue */
	size_t count;
} queue_t;

/*!
 \brief Initialize a new, empty, \c queue_t object
 \return initialized queue
 */
queue_t *queueNew();

/*!
 \brief Insert \a data at the end of the \a queue
 \param queue queue to insert data into
 \param data data to be inserted
 \param func function to be called to free \a data
 */
void queueInsert(queue_t *queue, void *data, queueNodeFreeFunction func);

/*!
 \brief Remove element from the front of the \a queue
 \param queue queue from which to remove the element
 \param data pointer to which the data from the head node should be assigned
 \return \c 1 on success, \c 0 otherwise
 */
int queueRemove(queue_t *queue, void **data);

/*!
 \brief Return amount of elements in the queue
 \param queue queue to be counted
 \return amount of elements in queue
 */
size_t queueCount(queue_t *queue);

/*!
 \brief Free memory taken up by \a queue

 If \a freeFunction is not \c NULL the node data will also be freed

 \param queue queue to be freed
 */
void queueFree(queue_t *queue);

/*!
 \}
 */

#endif
