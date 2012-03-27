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

void clear_screen() {
    putchar('');
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
