/*
 * utils.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120228
 * Lab:    2
 */

#include <stdio.h>
#include <string.h>
#include "uart.h"

void clear_screen() {
    uart_putch(&uart0, 12);
}

char * trim_string(char *str) {
    int i;
    char *trimmed;

    /* set start of string */
    for (i = 0; str[i] == ' '; i++);
    trimmed = &str[i];

    /* set end of string */
    for (i = strlen(trimmed); trimmed[i - 1] == ' '; i--);
    trimmed[i] = '\0';

    return trimmed;
}

void delete_char(char *str) {
    int i;

    for (i = 0; i < strlen(str); i++) {
        str[i] = str[i + 1];
    }
}
