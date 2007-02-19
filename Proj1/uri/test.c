#include <stdio.h>
#include "uri/uri.h"
#include "uri/server_authority.h"

void print_uri(char *uri_string) {
	uri_t *uri;
	server_authority_t *server_authority;
	
	if (!(uri = uri_parse(uri_string))) {
		printf("Invalid URI: %s\n", uri_string);
		return;
	}

	if (!(server_authority = server_authority_parse(uri->authority))) {
		printf("Invalid Server Authority: %s\n", uri->authority);
		return;
	}

	printf("scheme: %s\n", uri->scheme);
	printf("userinfo: %s\n", server_authority->userinfo);
	printf("host: %s\n", server_authority->host);
	printf("port: %d\n", server_authority->port);
	printf("path: %s\n", uri->path);
	printf("query: %s\n", uri->query);
	printf("fragment: %s\n\n", uri->fragment);

	server_authority_free(server_authority);
	uri_free(uri);
}

int main() {
	print_uri("http://www.ics.uci.edu/pub/ietf/uri/#Related");
	print_uri("file:///home/jlee?strange=query");
	print_uri("www.google.com");
	print_uri("http://MrStaticVoid@easynews.com:81/download?thisfile");

	return 0;
}
