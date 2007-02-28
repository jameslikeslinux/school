#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include "http.h"

void http_init(http_t *http, http_method method, http_url_t *url, void *post_data, size_t post_data_size, log_t *log) {
	http->status = DISCONNECTED;
	http->method = method;
	http->url = url;
	http->post_data = post_data;
	http->post_data_size = post_data_size;
	http->log = log;
}

int http_connect(http_t *http) {
	struct timeval starttime, endtime;
	struct hostent *he;
	struct sockaddr_in addr;
	char *ip;

	if (http->status >= CONNECTED) {
		log_message(http->log, INFO, "http_connect", "Already connected");
		return FAIL;
	}

	gettimeofday(&starttime, NULL);

	log_printf(http->log, DETAILS, "http_connect", "Connecting to %s:%d...", http->url->host, http->url->port);

	log_printf(http->log, DETAILS, "http_connect", "Calling gethostbyname");
	if (!(he = gethostbyname(http->url->host))) {
		log_perror(http->log, "gethostbyname");
		return FAIL;
	}

	ip = inet_ntoa(*((struct in_addr*) he->h_addr));
	if (strcmp(ip, http->url->host))
		log_printf(http->log, DETAILS, "http_connect", "%s is %s", http->url->host, ip);

	log_printf(http->log, DETAILS, "http_connect", "Calling socket");
	if ((http->sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		log_perror(http->log, "socket");
		return FAIL;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(http->url->port);
	addr.sin_addr = *((struct in_addr*) he->h_addr);
	memset(&addr.sin_zero, '\0', 8);

	log_printf(http->log, DETAILS, "http_connect", "Calling connect");
	if (connect(http->sockfd, (struct sockaddr*) &addr, sizeof(struct sockaddr)) == -1) {
		log_perror(http->log, "connect");
		return RETRY;
	}

	close(http->sockfd);

	gettimeofday(&endtime, NULL);
	log_printf(http->log, DETAILS, "http_connect", "Connected in %d ms.", endtime.tv_usec / 1000 - starttime.tv_usec / 1000);

	http->status = CONNECTED;
}
