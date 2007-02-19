#ifndef SERVER_AUTHORITY_H
#define SERVER_AUTHORITY_H

typedef struct {
	char *userinfo;
	char *host;
	int port;
} server_authority_t;

server_authority_t* server_authority_parse(char *string);
void server_authority_free(server_authority_t *server_authority);

#endif
