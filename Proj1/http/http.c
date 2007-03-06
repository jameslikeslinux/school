#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include "http.h"

#define RETRY_SLEEP 5
#define HOST_HEADER "Host: "
#define CONTENT_LENGTH_HEADER "Content-Length: "

static char *http_method_strings[] = {"GET", "HEAD", "POST"};

void http_init(http_t *http, http_url_t *url, http_method method, void *post_data, size_t post_data_size, int num_retry, log_t *log) {
	http->status = DISCONNECTED;
	http->url = url;
	http->method = method;
	http->post_data = post_data;
	http->post_data_size = post_data_size;
	http->num_retry = num_retry;
	http->log = log;
	http->sockfd = -1;
	http->header = NULL;
}

void http_run(http_t *http) {
	int ret, connect_try, request_try, recv_header_try;

	connect_try = 0;
	request_try = 0;
	recv_header_try = 0;

CONNECT:
	connect_try++;
	ret = http_connect(http);

	if (ret == FAIL || connect_try >= http->num_retry)
		return;
	else if (ret == RETRY) {
		log_printf(http->log, INFO, "http_run", "Retrying http_connect %d of %d times in %d seconds...", connect_try, http->num_retry, RETRY_SLEEP);
		sleep(RETRY_SLEEP);
		goto CONNECT;
	} else
		connect_try = 0;

SEND_REQUEST:
	request_try++;
	ret = http_send_request(http);

	if (ret == FAIL || request_try >= http->num_retry)
		return;
	else if (ret == RETRY) {
		http_disconnect(http);
		log_printf(http->log, INFO, "http_run", "Retrying http_send_request %d of %d times in %d seconds...", request_try, http->num_retry, RETRY_SLEEP);
		sleep(RETRY_SLEEP);
		goto CONNECT;
	} else
		request_try = 0;

RECV_HEADER:
	recv_header_try++;
	ret = http_recv_header(http);

	if (ret == FAIL || recv_header_try >= http->num_retry)
		return;
	else if (ret == RETRY) {
		http_disconnect(http);
		log_printf(http->log, INFO, "http_run", "Retrying http_recv_header %d of %d times in %d seconds...", recv_header_try, http->num_retry, RETRY_SLEEP);
		sleep(RETRY_SLEEP);
		goto CONNECT;
	} else
		recv_header_try = 0;
}

int http_connect(http_t *http) {
	struct timeval starttime, endtime;
	struct hostent *he;
	struct sockaddr_in addr;
	char *ip;

	if (http->status >= CONNECTED) {
		log_printf(http->log, INFO, "http_connect", "Already connected");
		return FAIL;
	}

	gettimeofday(&starttime, NULL);

	log_printf(http->log, DETAILS, "http_connect", "Connecting to %s:%d...", http->url->host, http->url->port);

	log_printf(http->log, DETAILS, "http_connect", "Calling gethostbyname");
	if (!(he = gethostbyname(http->url->host))) {
		log_herror(http->log, "gethostbyname");
		if (h_errno == TRY_AGAIN)
			return RETRY;
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

	gettimeofday(&endtime, NULL);
	log_printf(http->log, DETAILS, "http_connect", "Connected in %d ms.", endtime.tv_usec / 1000 - starttime.tv_usec / 1000);

	http->status = CONNECTED;

	return SUCCESS;
}

int http_send(http_t *http, void *data, size_t size) {
	int sent, pos;

	if (http->status == DISCONNECTED) {
		log_printf(http->log, INFO, "http_send", "Disconnected");
		return FAIL;
	}

	for (pos = 0; pos < size; pos += sent) {
		log_printf(http->log, DETAILS, "http_send", "Calling send");
		if ((sent = send(http->sockfd, data + pos, size - pos, 0)) == -1) {
			log_perror(http->log, "send");
			return RETRY;
		}
	}

	return SUCCESS;
}

int http_send_request(http_t *http) {
	struct timeval starttime, endtime;
	char *request_line;
	int ret;

	if (http->status == DISCONNECTED) {
		log_printf(http->log, INFO, "http_send_request", "Disconnected");
		return FAIL;
	} else if (http->status > CONNECTED) {
		log_printf(http->log, INFO, "http_send_request", "Request already sent");
		return FAIL;
	}
	
	gettimeofday(&starttime, NULL);

	log_printf(http->log, DETAILS, "http_send_request", "Sending %s request...", http_method_strings[http->method]);

	request_line = (char*) malloc(strlen(http->url->abs_path) + strlen(http->url->query) + 18);
	sprintf(request_line, "%s %s?%s HTTP/1.0\r\n", http_method_strings[http->method], http->url->abs_path, http->url->query);
	ret = http_send(http, request_line, strlen(request_line));
	free(request_line);
	if (ret) return ret;

	request_line = (char*) malloc(strlen(HOST_HEADER) + strlen(http->url->host) + 3);
	sprintf(request_line, "%s%s\r\n", HOST_HEADER, http->url->host);
	ret = http_send(http, request_line, strlen(request_line));
	free(request_line);
	if (ret) return ret;

	if (http->method == POST) {
		request_line = (char*) malloc(strlen(CONTENT_LENGTH_HEADER) + 15);
		sprintf(request_line, "%s%d\r\n\r\n", CONTENT_LENGTH_HEADER, http->post_data_size);
		ret = http_send(http, request_line, strlen(request_line));
		free(request_line);
		if (ret) return ret;

		if (ret = http_send(http, http->post_data, http->post_data_size)) return ret;
	}
	
	http_send(http, "\r\n", 2);

	gettimeofday(&endtime, NULL);
	log_printf(http->log, DETAILS, "http_send_request", "Request sent in %d ms.", endtime.tv_usec / 1000 - starttime.tv_usec / 1000);

	http->status = REQUEST_SENT;

	return SUCCESS;
}

int http_recv(http_t *http, void *buf, size_t size, int flags) {
	int ret;

	if (http->status == DISCONNECTED) {
		log_printf(http->log, INFO, "http_recv", "Disconnected");
		return FAIL;
	}

	log_printf(http->log, DETAILS, "http_recv", "Calling recv");
	if ((ret = recv(http->sockfd, buf, size, flags)) == -1) {
		log_perror(http->log, "recv");
		return FAIL;
	}

	return ret;
}

int http_recv_header(http_t *http) {
	struct timeval starttime, endtime;
	int size, read;
	char *buf, *blankline;

	if (http->status == DISCONNECTED) {
		log_printf(http->log, INFO, "http_recv_header", "Disconnected");
		return FAIL;
	} else if (http->status > HEADER_RECEIVED) {
		log_printf(http->log, INFO, "http_recv_header", "Header already received");
		return FAIL;
	}
	
	gettimeofday(&starttime, NULL);

	log_printf(http->log, DETAILS, "http_recv_header", "Receiving HTTP response header...");

	size = 1024;
	buf = (char*) malloc(size);

	while (1) {
		if ((read = http_recv(http, buf, size - 1, MSG_PEEK)) < 0) {
			free(buf);
			return read;
		}
		buf[read] = '\0';

		if (blankline = strstr(buf, "\r\n\r\n")) {
			size = blankline - buf + 5;
			http->header = (char*) malloc(size);

			read = http_recv(http, http->header, size - 1, 0);
			if (read >= 0 && read < size - 1) {
				log_printf(http->log, INFO, "http_recv_header", "Error receiving full header");
				free(http->header);
				http->header = NULL;
				return FAIL;
			} else if (read < 0) {
				free(http->header);
				http->header = NULL;
				return read;
			}

			http->header[size - 1] = '\0';
			break;
		} else if (read == size - 1) {
			free(buf);
			size *= 2;
			buf = (char*) malloc(size);
		}
	}

	free(buf);

	buf = strtok(http->header, "\r\n");
	while (buf != NULL) {
		log_printf(http->log, DETAILS, "http_recv_header", "%s", buf);
		buf = strtok(NULL, "\r\n");
	}

	gettimeofday(&endtime, NULL);
	log_printf(http->log, DETAILS, "http_recv_header", "Header received in %d ms.", endtime.tv_usec / 1000 - starttime.tv_usec / 1000);

	http->status = HEADER_RECEIVED;

	return SUCCESS;
}

int http_disconnect(http_t *http) {
	struct timeval starttime, endtime;

	if (http->status == DISCONNECTED) {
		log_printf(http->log, INFO, "http_disconnect", "Already disconnected");
		return FAIL;
	}
	
	gettimeofday(&starttime, NULL);

	log_printf(http->log, DETAILS, "http_disconnect", "Disconnecting...");

	log_printf(http->log, DETAILS, "http_disconnect", "Calling close");
	if (close(http->sockfd)) {
		log_perror(http->log, "close");
		return FAIL;
	}
	http->sockfd = -1;

	if (http->header)
		free(http->header);

	gettimeofday(&endtime, NULL);
	log_printf(http->log, DETAILS, "http_disconnect", "Disconnected in %d ms.", endtime.tv_usec / 1000 - starttime.tv_usec / 1000);

	http->status = DISCONNECTED;

	return SUCCESS;
}
