#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stddef.h>
#include <stdlib.h>

template <typename T>
class RingBuffer {
    public:
        RingBuffer(size_t size);
        ~RingBuffer();
        bool enqueue(const T &t) volatile;
        bool dequeue(T &t) volatile;
        bool peek(T &t) volatile;
        bool isEmpty() const volatile;
        bool isFull() const volatile;
        void clear() volatile;
        int available() volatile;

    private:
        T *buffer;
        size_t size;
        unsigned int head, tail;
};

template <typename T>
RingBuffer<T>::RingBuffer(size_t size) {
    this->buffer = (T *) malloc(size * sizeof(T));

    if (this->buffer) {
        this->size = size;
    } else {
        this->size = 1;
    }

    this->head = 0;
    this->tail = 0;
}

template <typename T>
RingBuffer<T>::~RingBuffer() {
    free(this->buffer);
}

template <typename T>
bool RingBuffer<T>::enqueue(const T &t) volatile {
    unsigned int i = (this->head + 1) % this->size;

    // if we should be storing the received character into the location
    // just before the tail (meaning that the head would advance to the
    // current location of the tail), we're about to overflow the buffer
    // and so we don't write the character or advance the head.
    if (i != this->tail) {
        this->buffer[this->head] = t;
        this->head = i; 
        return true;
    }   
        
    return false;
}

template <typename T>
bool RingBuffer<T>::dequeue(T &t) volatile {
    if (isEmpty())
        return false;

    t = this->buffer[this->tail];
    this->tail = (this->tail + 1) % this->size;
    return true;
}

template <typename T>
bool RingBuffer<T>::peek(T &t) volatile {
    if (isEmpty())
        return false;

    t = this->buffer[this->tail];
    return true;
}

template <typename T>
bool RingBuffer<T>::isEmpty() const volatile {
    return this->head == this->tail;
}

template <typename T>
bool RingBuffer<T>::isFull() const volatile {
    unsigned int i = (this->head + 1) % this->size;
    return i == this->tail;
}

template <typename T>
void RingBuffer<T>::clear() volatile {
    this->head = 0;
    this->tail = 0;
}

template <typename T>
int RingBuffer<T>::available() volatile {
    if (this->head > this->tail) {
        return this->head - this->tail;
    } else if (this->head < this->tail) {
        return this->size - (this->tail - this->head);
    }

    return 0;
}

#endif
