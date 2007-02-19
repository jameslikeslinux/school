#ifndef LIST_H
#define LIST_H

typedef struct {
	void *data;
	int size;
} node_t;

typedef struct {
	node_t *head;
	node_t *tail;
} list_t;

#endif
