#ifndef HTTP_H
#define HTTP_H

#include "http_url.h"
#include "log.h"

#define SUCCESS 0
#define FAIL 1
#define RETRY 2

#define BUF_SIZE 100

typedef enum { DISCONNECTED, CONNECTED, REQUEST_SENT, RESPONSE_RECEIVED } http_status;
typedef enum { GET, HEAD, POST } http_method;

typedef struct {
	http_status status;
	http_method method;
	http_url_t *url;
	void *post_data;
	size_t post_data_size;
	log_t *log;
	int sockfd;
	char buf[BUF_SIZE];
} http_t;

void http_init(http_t *http, http_method method, http_url_t *url, void *post_data, size_t post_data_size, log_t *log);
int http_connect(http_t *http);
int http_send_request(http_t *http);
int http_get_header(http_t *http);

#endif
