/*
 * queue.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120319
 * Lab:    3
 */

#ifndef QUEUE_H
#define QUEUE_H

typedef struct {
    /*
     * PRIAVATE
     */
    void **buffer;
    unsigned int size;
    volatile unsigned int head, tail;
} queue_t;

/*
 * Initialize a queue of size 'size'
 */
void queue_init(queue_t *queue, unsigned int size);

/*
 * Free the queue's dynamically allocated buffer
 */
void queue_free(queue_t *queue);

/*
 * Add arbitrary data to the end of the queue
 *
 * returns - 1 if added successfully, 0 otherwise
 */
int queue_enqueue(queue_t *queue, void *data);

/*
 * Remove the element at the head of the queue
 *
 * returns - 1 if dequeued successfully, 0 otherwise
 */
int queue_dequeue(queue_t *queue, void **data);


int queue_isempty(queue_t *queue);

#endif