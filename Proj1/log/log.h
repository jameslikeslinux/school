#ifndef LOG_H
#define LOG_H

#include <time.h>
#include <errno.h>
#include "list/list.h"

typedef enum { INFO, DETAILS } message_type;

typedef struct {
	time_t time;
	message_type type;
	char *source;
	char *description;
} message_t;

typedef list_t log_t;

void log_init(log_t *log);
void log_message(log_t *log, message_type type, char *source, char *description);
void log_perror(log_t *log, char *source);
void log_remove(log_t *log, node_t *message);
void log_clear(log_t *log);
void log_free(log_t *log);

#endif
