#include <stdio.h>
#include "list/list.h"

int main() {
	list_t list;
	list_init(&list);
	printf("head: %p\ntail: %p\n", list.head, list.tail);

	return 0;
}
