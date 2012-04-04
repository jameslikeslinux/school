/*
 * queue.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120319
 * Lab:    3
 */

#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
#include "uart.h"

/*
 * THIS IS A SIMPLE CIRCULAR QUEUE
 * ...enough said.
 */

void queue_init(queue_t *queue, unsigned int size) {
    queue->buffer = (void **) malloc(size * sizeof(void *));

    if (queue->buffer) {
        queue->size = size;
    } else {
        myprintf("Failed to allocate queue buffer\n");
        queue->size = 1;
    }

    queue->head = 0;
    queue->tail = 0;
}

void queue_free(queue_t *queue) {
    queue->size = 1;
    queue->head = 0;
    queue->tail = 0;

    free(queue->buffer);
    queue->buffer = NULL;
}

int queue_enqueue(queue_t *queue, void *data) {
    unsigned int i = (queue->head + 1) % queue->size;

    if (i != queue->tail) {
        queue->buffer[queue->head] = data;
        queue->head = i;
        return 1;
    }

    return 0;
}

int queue_dequeue(queue_t *queue, void **data) {
    if (queue_isempty(queue)) {
        return 0;
    }

    *data = queue->buffer[queue->tail];
    queue->tail = (queue->tail + 1) % queue->size;
    return 1;
}

int queue_isempty(queue_t *queue) {
    return queue->head == queue->tail;
}