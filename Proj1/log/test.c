#include <stdio.h>
#include "log.h"

static log_t log;

void print_log(log_t *log) {
	node_t *node;
	message_t *message;
	
	printf("---printing log---\n");

	for (node = log->messages.head; node; node = node->next) {
		message = (message_t*) node->data;
		printf("%s: %s\n", message->source, message->description);
	}

	printf("---end log---\n");
}

void log_callback() {
	printf("%s logged\n", ((message_t*) log.messages.tail->data)->source);
}

int main() {
	log_init(&log);
	log_register_message_callback(&log, log_callback);

	log_message(&log, DETAILS, "main", "this is a test");
	log_message(&log, INFO, "connect", "connection refused");
	log_message(&log, INFO, "read", "couldn't read");
	print_log(&log);

	printf("---clearing log---\n");
	log_clear(&log);
	print_log(&log);

	log_perror(&log, "main");
	print_log(&log);

	return 0;
}
