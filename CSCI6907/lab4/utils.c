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

#define TIMER_ENABLE (1 << 7)
#define TIMER_MODE 0    /* one-shot */
#define TIMER_PRESCALE 1

void delay_micros(int micros) {
    /* disable and reset timer */
    T2CTL = 0;

    /* set timer type mode */
    T2CTL0 |= ((TIMER_MODE >> 3) << 7);
    T2CTL1 |= ((TIMER_MODE & 0x7) << 0);

    /* set prescaler */
    T2CTL1 |= ((TIMER_PRESCALE & 0x7) << 3);

    /* start counter at 2 */
    T2HL = 2;
    
    /* set reload value */
    /* reload = micros * (hz / (micros/s * prescale)) + start - 1 */
    T2R = micros * (18432000.0 / (1000000.0 * TIMER_PRESCALE)) + 1;

    T2CTL |= TIMER_ENABLE;

    while (T2HL != 1);
}