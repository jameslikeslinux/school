#include <stdlib.h>
#include <string.h>
#include "log/log.h"

void log_init(log_t *log) {
	list_init(log);
}

void log_message(log_t *log, message_type type, char *source, char *description) {
	message_t *message;

	message = (message_t*) malloc(sizeof(message_t));
	message->time = time(NULL);
	message->type = type;

	message->source = (char*) malloc(strlen(source) + 1);
	strcpy(message->source, source);

	message->description = (char*) malloc(strlen(description) + 1);
	strcpy(message->description, description);

	list_insert_end(log, message);
}

void log_perror(log_t *log, char *source) {
	log_message(log, INFO, source, strerror(errno));
}

void log_remove(log_t *log, node_t *node) {
	message_t *message = (message_t*) node->data;
	free(message->description);
	free(message->source);
	free(message);
	list_remove(log, node);
}

void log_clear(log_t *log) {
	node_t *node;
	for (node = log->head; node; node = node->next)
		log_remove(log, node);
}

void log_free(log_t *log) {
	log_clear(log);
}
