#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include "uri/server_authority.h"

#define SERVER_AUTHORITY_REGEX "(([^?#]*)@)?([^?#:]*)(:([0-9]*))?"
#define SERVER_AUTHORITY_MATCHES 6

int server_authority_parse(server_authority_t *server_authority, char *string) {
	regex_t regex;
	regmatch_t matches[SERVER_AUTHORITY_MATCHES];
	int ret, size;
	char *port;

	regcomp(&regex, SERVER_AUTHORITY_REGEX, REG_EXTENDED);
	ret = regexec(&regex, string, SERVER_AUTHORITY_MATCHES, matches, 0);
	regfree(&regex);
	if (ret)
		return INVALID_SERVER_AUTHORITY;

	size = (int) matches[2].rm_eo - matches[2].rm_so;
	server_authority->userinfo = (char*) malloc(size + 1);
	strncpy(server_authority->userinfo, string + matches[2].rm_so, size);
	server_authority->userinfo[size] = '\0';

	size = (int) matches[3].rm_eo - matches[3].rm_so;
	server_authority->host = (char*) malloc(size + 1);
	strncpy(server_authority->host, string + matches[3].rm_so, size);
	server_authority->host[size] = '\0';

	size = (int) matches[5].rm_eo - matches[5].rm_so;
	if (size == 0)
		server_authority->port = -1;
	else {
		port = (char*) malloc(size + 1);
		strncpy(port, string + matches[5].rm_so, size);
		port[size] = '\0';
		server_authority->port = atoi(port);
		free(port);
	}

	return 0;
}

void server_authority_free(server_authority_t *server_authority) {
	free(server_authority->host);
	free(server_authority->userinfo);
}
