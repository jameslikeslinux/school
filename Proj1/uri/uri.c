#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include "uri/uri.h"

/* Regex from RFC 2396 */
#define URI_REGEX "^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?"
#define URI_MATCHES 10

int uri_parse(uri_t *uri, char *string) {
	regex_t regex;
	regmatch_t matches[URI_MATCHES];
	int ret, size;

	regcomp(&regex, URI_REGEX, REG_EXTENDED);
	ret = regexec(&regex, string, URI_MATCHES, matches, 0);
	regfree(&regex);
	if (ret)
		return INVALID_URI;

	size = (int) matches[2].rm_eo - matches[2].rm_so;
	uri->scheme = (char*) malloc(size + 1);
	strncpy(uri->scheme, string + matches[2].rm_so, size);
	uri->scheme[size] = '\0';

	size = (int) matches[4].rm_eo - matches[4].rm_so;
	uri->authority = (char*) malloc(size + 1);
	strncpy(uri->authority, string + matches[4].rm_so, size);
	uri->authority[size] = '\0';

	size = (int) matches[5].rm_eo - matches[5].rm_so;
	uri->path = (char*) malloc(size + 1);
	strncpy(uri->path, string + matches[5].rm_so, size);
	uri->path[size] = '\0';

	size = (int) matches[7].rm_eo - matches[7].rm_so;
	uri->query = (char*) malloc(size + 1);
	strncpy(uri->query, string + matches[7].rm_so, size);
	uri->query[size] = '\0';

	size = (int) matches[9].rm_eo - matches[9].rm_so;
	uri->fragment = (char*) malloc(size + 1);
	strncpy(uri->fragment, string + matches[9].rm_so, size);
	uri->fragment[size] = '\0';

	return 0;
}

void uri_free(uri_t *uri) {
	free(uri->fragment);
	free(uri->query);
	free(uri->path);
	free(uri->authority);
	free(uri->scheme);
}
