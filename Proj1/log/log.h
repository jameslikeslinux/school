#ifndef LOG_H
#define LOG_H

#include <time.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdarg.h>
#include "list.h"

typedef enum { INFO, DETAILS } message_type;

typedef struct {
	time_t time;
	message_type type;
	char *source;
	char *description;
} message_t;

typedef struct {
	list_t messages;
	void (*message_callback)();
} log_t;

void log_init(log_t *log);
void log_message(log_t *log, message_type type, char *source, char *description);
void log_register_message_callback(log_t *log, void (*message_callback)());
void log_perror(log_t *log, char *source);
void log_printf(log_t *log, message_type type, char *source, char *format, ...);
void log_remove(log_t *log, node_t *message);
void log_clear(log_t *log);
void log_free(log_t *log);

#endif
