#include <stdlib.h>
#include "list/list.h"

void list_init(list_t *list) {
	list->head = NULL;
	list->tail = NULL;
	sem_init(&list->sem, 0, 1);
}

void list_insert_after(list_t *list, node_t *node, void *data) {
	node_t *new_node;

	sem_wait(&list->sem);

	new_node = (node_t*) malloc(sizeof(node_t));
	new_node->data = data;

	new_node->prev = node;
	new_node->next = node->next;

	if (!node->next)
		list->tail = new_node;
	else
		node->next->prev = new_node;

	node->next = new_node;

	sem_post(&list->sem);
}

void list_insert_before(list_t *list, node_t *node, void *data) {
	node_t *new_node;

	sem_wait(&list->sem);

	new_node = (node_t*) malloc(sizeof(node_t));
	new_node->data = data;

	new_node->prev = node->prev;
	new_node->next = node;

	if (!node->prev)
		list->head = new_node;
	else
		node->prev->next = new_node;

	node->prev = new_node;

	sem_post(&list->sem);
}

void list_insert_beginning(list_t *list, void *data) {
	if (!list->head) {
		node_t *new_node;

		sem_wait(&list->sem);

		new_node = (node_t*) malloc(sizeof(node_t));
		new_node->data = data;

		list->head = new_node;
		list->tail = new_node;
		new_node->prev = NULL;
		new_node->next = NULL;

		sem_post(&list->sem);
	} else
		list_insert_before(list, list->head, data);
}

void list_insert_end(list_t *list, void *data) {
	if (!list->tail)
		list_insert_beginning(list, data);
	else
		list_insert_after(list, list->tail, data);
}

void list_remove(list_t *list, node_t *node) {
	sem_wait(&list->sem);

	if (!node->prev)
		list->head = node->next;
	else
		node->prev->next = node->next;

	if (!node->next)
		list->tail = node->prev;
	else
		node->next->prev = node->prev;

	free(node);

	sem_post(&list->sem);
}

void list_free(list_t *list) {
	while (list->head)
		list_remove(list, list->head);
	sem_destroy(&list->sem);
}
