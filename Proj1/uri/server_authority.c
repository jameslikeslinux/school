#include <stdlib.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include "uri/server_authority.h"

#define SERVER_AUTHORITY_REGEX "(([^?#]*)@)?([^?#:]*)(:([0-9]*))?"
#define SERVER_AUTHORITY_MATCHES 6

server_authority_t* server_authority_parse(char *string) {
	regex_t regex;
	regmatch_t matches[SERVER_AUTHORITY_MATCHES];
	server_authority_t *server_authority;
	int size;
	char *port;

	regcomp(&regex, SERVER_AUTHORITY_REGEX, REG_EXTENDED);
	if (regexec(&regex, string, SERVER_AUTHORITY_MATCHES, matches, 0))
		return NULL;

	server_authority = (server_authority_t*) malloc(sizeof(server_authority_t));

	size = (int) matches[2].rm_eo - matches[2].rm_so;
	server_authority->userinfo = (char*) malloc(size + 1);
	strncpy(server_authority->userinfo, string + matches[2].rm_so, size);
	server_authority->userinfo[size] = '\0';

	size = (int) matches[3].rm_eo - matches[3].rm_so;
	server_authority->host = (char*) malloc(size + 1);
	strncpy(server_authority->host, string + matches[3].rm_so, size);
	server_authority->host[size] = '\0';

	size = (int) matches[5].rm_eo - matches[5].rm_so;
	port = (char*) malloc(size + 1);
	strncpy(port, string + matches[5].rm_so, size);
	port[size] = '\0';
	server_authority->port = atoi(port);
	free(port);

	return server_authority;
}

void server_authority_free(server_authority_t *server_authority) {
	free(server_authority->host);
	free(server_authority->userinfo);
	free(server_authority);
}
