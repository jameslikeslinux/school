/*
 * utils.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120228
 * Lab:    2
 */

#include <zneo.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
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

#define MICROS_TIMER_ENABLE (1 << 7)
#define MICROS_TIMER_MODE 0    /* one-shot */
#define MICROS_TIMER_PRESCALE 0x0

/* reload is 18.432 * micros */
void delay_micros(unsigned short reload) {
    /* disable and reset timer */
    T2CTL = 0;

    /* set timer type mode */
    T2CTL0 |= ((MICROS_TIMER_MODE >> 3) << 7);
    T2CTL1 |= ((MICROS_TIMER_MODE & 0x7) << 0);

    /* set prescaler */
    T2CTL1 |= ((MICROS_TIMER_PRESCALE & 0x7) << 3);

    /* start counter at 1 */
    T2HL = 1;
    
    /* set reload value */
    /* XXX: hardcoded for efficiency... */
    /* subtract overhead of function...value of 46 determined by oscilloscope testing */
    T2R = reload - 46;

    /* enable timer */
    T2CTL |= MICROS_TIMER_ENABLE;

    /* wait for timer to disable itself */
    while (T2CTL & MICROS_TIMER_ENABLE);
}

int hex_string_to_integer(char *str) {
    int i, num, digit;
    char c;

    num = 0;

    for (i = 0; i < strlen(str); i++) {
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