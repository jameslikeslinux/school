#include <stdio.h>
#include "uri/uri.h"

int main() {
	uri_t *uri = uri_parse("http://www.ics.uci.edu/pub/ietf/uri/#Related");
	printf("scheme: %s\n", uri->scheme);
	printf("authority: %s\n", uri->authority);
	printf("path: %s\n", uri->path);
	printf("query: %s\n", uri->query);
	printf("fragment: %s\n", uri->fragment);
	uri_free(uri);

	return 0;
}
