/*
 * led.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120220
 * Lab:    1
 */

#ifndef LED_H
#define LED_H

/* 
 * Define an extra column to account for space between each panel in
 * the LED array.  This has two benefits: first, it causes the leftmost
 * column to be shifted to a nonprintable area when smooth scrolling so
 * characters appear to "flow" between panels without jumping; and second,
 * the "dummy" column creates a space when shifted, so spaces automatically
 * get put between characters when smooth scrolling without me having to
 * do anything :)
 */
#define LED_NUM_COLS 6
#define LED_NUM_ROWS 7

typedef struct {
    /*
     * PRIVATE
     */
    unsigned char volatile near *port;
    int bit;
    int row;
    unsigned char character[7];
} led_t;

/*
 * Initialize an led_t object representing a single LED panel
 *
 * port - the port connected to the LED's latch clock line
 * bit - a bitmask for the port
 * charecter - what should be displayed upon initialization (usually ' ')
 */ 
void led_init(led_t *led, unsigned char volatile near *port, int bit, char character);

/*
 * Set the character to be displayed
 * 
 * newchar - an ASCII character to be displayed
 */
void led_set_char(led_t *led, char newchar);

/*
 * Shift the pixels by one column from right to left
 * (used for smooth scrolling)
 */
void led_shift(led_t *left, led_t *right);

#endif
