#include <string.h>
#include "Utils.h"

void deleteChar(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        str[i] = str[i + 1];
    }
}
