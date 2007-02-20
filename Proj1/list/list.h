#ifndef LIST_H
#define LIST_H

typedef struct node_s {
	void *data;
	struct node_s *prev;
	struct node_s *next;
} node_t;

typedef struct {
	node_t *head;
	node_t *tail;
} list_t;

void list_init(list_t *list);
void list_insert_after(list_t *list, node_t *node, void *data);
void list_insert_before(list_t *list, node_t *node, void *data);
void list_insert_beginning(list_t *list, void *data);
void list_insert_end(list_t *list, void *data);
void list_remove(list_t *list, node_t *node);
void list_free(list_t *list);

#endif
