#include <stdio.h>
#include "list/list.h"

int main() {
	list_t list;
	printf("head: %p\ntail: %p\n", list.head, list.tail);

	return 0;
}
