#ifndef URI_H
#define URI_H

typedef struct {
	char *scheme;
	char *authority;
	char *path;
	char *query;
	char *fragment;
} uri_t;

uri_t* uri_parse(char *string);
void uri_free(uri_t *uri);

#endif
