#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "Utils.h"

void deleteChar(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        str[i] = str[i + 1];
    }
}

int hexStringToInteger(char *str) {
    int num, digit;
    char c;

    num = 0;

    for (int i = 0; i < strlen(str); i++) {
        c = toupper(str[i]);

        if (c >= 'A' && c <= 'F') {
            digit = c - 'A' + 10;
        } else if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else {
            return -1;
        }

        num |= digit << (4 * (strlen(str) - i - 1));
    }

    return num;
}

byte * stringToMacAddress(char *str) {
    char *token;
    int tokenCount, octet;
    static byte value[6];

    tokenCount = 0;
    token = strtok(str, ":");
    while (token) {
        if (++tokenCount > 6) {
            return NULL;
        }

        if (strlen(token) != 2) {
            return NULL;
        }

        if ((octet = hexStringToInteger(token)) != -1) {
            value[tokenCount - 1] = octet;
        }

        token = strtok(NULL, ":");
    }

    if (tokenCount != 6) {
        return NULL;
    }

    return value;
}

byte * stringToIpAddress(char *str) {
    char *token;
    int tokenCount, octet;
    static byte value[4];

    tokenCount = 0;
    token = strtok(str, ".");
    while (token) {
        if (++tokenCount > 4) {
            return NULL;
        }

        if ((octet = atoi(token)) != 0 || !strcmp(token, "0")) {
            value[tokenCount - 1] = octet;
        }

        token = strtok(NULL, ".");
    }

    if (tokenCount != 4) {
        return NULL;
    }

    return value;
}

char * macAddressToString(byte *mac) {
    static char str[18];
    sprintf(str, "%2x:%2x:%2x:%2x:%2x:%2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return str;    
}

char * ipAddressToString(byte *ip) {
    static char str[16];
    sprintf(str, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return str;    
}
