#include <stdio.h>
#include "uri/http_url.h"

void print_uri(char *uri_string) {
	http_url_t http_url;

	switch (http_url_parse(&http_url, uri_string)) {
		case INVALID_URI:
			printf("Invalid URI\n\n");
			return;

		case INVALID_SERVER_AUTHORITY:
			printf("Invalid Server Authority\n\n");
			return;

		case INVALID_SCHEME:
			printf("Invalid Scheme\n\n");
			return;

		case INVALID_PORT:
			printf("Invalid Port\n\n");
			return;
	}

	printf("userinfo: %s\n", http_url.userinfo);
	printf("host: %s\n", http_url.host);
	printf("port: %d\n", http_url.port);
	printf("abs_path: %s\n", http_url.abs_path);
	printf("query: %s\n", http_url.query);
	printf("fragment: %s\n\n", http_url.fragment);

	http_url_free(&http_url);
}

int main() {
	print_uri("http://www.ics.uci.edu/pub/ietf/uri/#Related");
	print_uri("file:///home/jlee?strange=query");
	print_uri("www.google.com");
	print_uri("http://MrStaticVoid@easynews.com:81/download?thisfile");
	print_uri("http://test");

	return 0;
}
