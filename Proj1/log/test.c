#include <stdio.h>
#include "log/log.h"

void print_log(log_t *log) {
	node_t *node;
	message_t *message;
	
	printf("---printing log---\n");

	for (node = log->head; node; node = node->next) {
		message = (message_t*) node->data;
		printf("%s: %s\n", message->source, message->description);
	}

	printf("---end log---\n");
}

int main() {
	log_t log;

	log_init(&log);
	log_message(&log, INFO, "main", "this is a test");
	log_message(&log, ERROR, "connect", "connection refused");
	log_message(&log, WARNING, "read", "couldn't read");
	print_log(&log);

	printf("---clearing log---\n");
	log_clear(&log);
	print_log(&log);

	return 0;
}
