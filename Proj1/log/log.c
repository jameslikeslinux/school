#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "log.h"

void log_init(log_t *log) {
	list_init(&log->messages);
	log->message_callback = NULL;
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

	list_insert_end(&log->messages, message);

	if (log->message_callback)
		log->message_callback();
}

void log_register_message_callback(log_t *log, void (*message_callback)()) {
	log->message_callback = message_callback;
}

void log_perror(log_t *log, char *source) {
	log_message(log, INFO, source, strerror(errno));
}

void log_herror(log_t *log, char *source) {
	log_message(log, INFO, source, hstrerror(h_errno));
}

void log_printf(log_t *log, message_type type, char *source, char *format, ...) {
	char description[255];
	va_list args;
	
	va_start(args, format);
	vsprintf(description, format, args);
	va_end(args);

	log_message(log, type, source, description);
}

void log_remove(log_t *log, node_t *node) {
	message_t *message = (message_t*) node->data;
	free(message->description);
	free(message->source);
	free(message);
	list_remove(&log->messages, node);
}

void log_clear(log_t *log) {
	node_t *node;
	for (node = log->messages.head; node; node = node->next)
		log_remove(log, node);
}

void log_free(log_t *log) {
	log_clear(log);
}
