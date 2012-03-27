/*
 * led.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120220
 * Lab:    1
 */

#include <zneo.h>
#include <string.h>
#include "led.h"
#include "matrix.h"
#include "timer.h"

#define LED_SCAN_RATE 2

void _led_cycle(led_t *led) {
    /* push the data through the latch */
    *(led->port) &= ~(led->bit);
    *(led->port) |= led->bit;
}

void _led_scan_row(void *data) {
    led_t *led = (led_t *) data;
    
    PEOUT |= 0x1f;  /* set column bits high */
    PEOUT &= ~(led->character[led->row]);  /* set column bits low where needed to display on */
    
    PGOUT &= ~0x7f; /* set row bits low */
    PGOUT |= 1 << led->row; /* set row bits high on the current scanned row */
    
    led->row = (led->row + 1) % LED_NUM_ROWS;
    
    _led_cycle(led);
}

void led_init(led_t *led, unsigned char volatile near *port, int bit, char character) {
    /* set data direction out on both ports used by LEDs */
    PEDD = 0;
    PGDD = 0;
    
    led->port = port;
    led->bit = bit;
    led->row = 0;
    led_set_char(led, character);
    
    if (port) {
        timer_add_callback(_led_scan_row, led, LED_SCAN_RATE, 0);
    }   /* else -> no need to scan on a dummy LED */
}

void led_set_char(led_t *led, char newchar) {
    if (newchar < 0x20 || newchar > 0x7e) {
        newchar = 0x20;
    }
    
    memcpy(led->character, char_data[newchar - 0x20], LED_NUM_ROWS);
}

void led_shift(led_t *left, led_t *right) {
    int row;
    
    for (row = 0; row < LED_NUM_ROWS; row++) {
        if (right) {
            left->character[row] = ((left->character[row] << 1) | (right->character[row] >> (LED_NUM_COLS - 1)));
        } else {
            left->character[row] <<= 1;
        }
    }   
}
