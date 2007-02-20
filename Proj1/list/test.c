#include <stdio.h>
#include "list/list.h"

int main() {
	char *foo = "foo", *bar = "bar", *baz = "baz";
	list_t list;
	node_t *node;

	list_init(&list);
	list_insert_end(&list, foo);
	list_insert_end(&list, bar);
	list_insert_end(&list, baz);

	for (node = list.head; node; node = node->next)
		printf("%s\n", node->data);

	list_free(&list);

	return 0;
}
