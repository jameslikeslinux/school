/*
 * keyboard.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120321
 * Lab:    4
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef struct {
    unsigned volatile char near *in, *out;
    unsigned char clock_bit, data_bit;
    volatile int stop;
    short keypress;
} keyboard_t;

void keyboard_init(keyboard_t *this, unsigned volatile char near *in, unsigned volatile char near *out, unsigned volatile char near *dd, unsigned volatile char near *oc, unsigned char clock_bit, unsigned char data_bit);

/*
 * returns raw scan code from keyboard.
 * blocks until keycode is read or keyboard_stop called
 */
char keyboard_get_scancode(keyboard_t *this);

/*
 * returns a keycode with modifier information (such as ctrl and shift)
 * encoded in the most significant byte
 */
short keyboard_get_keypress(keyboard_t *this);

/*
 * returns a string (usually of just one character) of a keypress
 */ 
char * keyboard_get_string(keyboard_t *this);

/*
 * unblock the above getters
 */
void keyboard_stop(keyboard_t *this);

#endif