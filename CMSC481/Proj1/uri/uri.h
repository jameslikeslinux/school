#ifndef URI_H
#define URI_H

#define INVALID_URI 1

typedef struct {
	char *scheme;
	char *authority;
	char *path;
	char *query;
	char *fragment;
} uri_t;

void uri_init(uri_t *uri);
int uri_parse(uri_t *uri, char *string);
void uri_free(uri_t *uri);

#endif
