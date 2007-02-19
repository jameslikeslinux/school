#ifndef SERVER_AUTHORITY_H
#define SERVER_AUTHORITY_H

#define INVALID_SERVER_AUTHORITY 2

typedef struct {
	char *userinfo;
	char *host;
	int port;
} server_authority_t;

int server_authority_parse(server_authority_t *server_authority, char *string);
void server_authority_free(server_authority_t *server_authority);

#endif
