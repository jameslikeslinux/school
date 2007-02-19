#ifndef HTTP_URL_H
#define HTTP_URL_H

#include "uri/uri.h"
#include "uri/server_authority.h"

#define INVALID_SCHEME 3
#define INVALID_PORT 4

typedef struct {
	char *userinfo;
	char *host;
	int port;
	char *abs_path;
	char *query;
	char *fragment;
} http_url_t;

void http_url_init(http_url_t *http_url);
int http_url_parse(http_url_t *http_url, char *string);
void http_url_free(http_url_t *http_url);

#endif
