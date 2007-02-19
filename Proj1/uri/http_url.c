#include <stdlib.h>
#include <string.h>
#include "uri/http_url.h"

int http_url_parse(http_url_t *http_url, char *string) {
	uri_t uri;
	server_authority_t server_authority;
	int ret;

	if ((ret = uri_parse(&uri, string)))
		return ret;
	
	if (strcmp(uri.scheme, "http"))
		return INVALID_SCHEME;
	
	if ((ret = server_authority_parse(&server_authority, uri.authority))) {
		uri_free(&uri);
		return ret;
	}
	
	http_url->userinfo = (char*) malloc(strlen(server_authority.userinfo) + 1);
	strcpy(http_url->userinfo, server_authority.userinfo);

	http_url->host = (char*) malloc(strlen(server_authority.host) + 1);
	strcpy(http_url->host, server_authority.host);

	if (server_authority.port == -1)
		http_url->port = 80;
	else if (server_authority.port < -1 && server_authority.port > 65535) {
		free(http_url->host);
		free(http_url->userinfo);
		server_authority_free(&server_authority);
		uri_free(&uri);
		return INVALID_PORT;
	} else
		http_url->port = server_authority.port;
	
	if (strlen(uri.path) == 0) {
		http_url->abs_path = (char*) malloc(2);
		strcpy(http_url->abs_path, "/");
	} else {
		http_url->abs_path = (char*) malloc(strlen(uri.path) + 1);
		strcpy(http_url->abs_path, uri.path);
	}

	http_url->query = (char*) malloc(strlen(uri.query) + 1);
	strcpy(http_url->query, uri.query);

	http_url->fragment = (char*) malloc(strlen(uri.fragment) + 1);
	strcpy(http_url->fragment, uri.fragment);

	return ret;
}

void http_url_free(http_url_t *http_url) {
	free(http_url->fragment);
	free(http_url->query);
	free(http_url->abs_path);
	free(http_url->host);
	free(http_url->userinfo);
}
