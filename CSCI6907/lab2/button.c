/*
 * button.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120220
 * Lab:    1
 */

#include <zneo.h>
#include <stdlib.h>
#include "button.h"
#include "timer.h"

/* check for button presses every millisecond */
#define BUTTON_CHECK_INTERVAL 1

/* ignore everything for 20 ms after a state change */
#define BUTTON_BOUNCE_IGNORE_TIME 20

/* count number of button presses 500 ms after a first press */
#define BUTTON_DOUBLE_PRESS_WINDOW 500

void _button_double_press_window_closed(void *data) {
    button_t *button = (button_t *) data;

    /* invoke callback corresponding to number of button presses */
    if (button->timespressed == 1 && button->single_press_callback) {
        button->single_press_callback(button->single_press_data);
    } else if (button->timespressed == 2 && button->double_press_callback) {
        button->double_press_callback(button->double_press_data);
    }

    button->timespressed = 0;
}

void _button_check_press(void *data) {
    long currenttime;
    button_t *button = (button_t *) data;

    currenttime = timer_get_uptime_millis();

    /* ignore everything for 20 ms after a state change for debouncing */
    if (currenttime - button->lastpressed > BUTTON_BOUNCE_IGNORE_TIME) {
        if (!(*(button->port) & button->bit)) {     /* button pressed */
            button->isdepressed = 1;
            button->lastpressed = currenttime;
        } else if (button->isdepressed) {           /* button released */
            button->isdepressed = 0;
            button->timespressed++;
            button->lastpressed = currenttime;

            if (button->timespressed == 1) {
                /* start timer for counting additional button presses */
                if (!timer_add_callback(_button_double_press_window_closed, button, BUTTON_DOUBLE_PRESS_WINDOW, 1)) {
                    /* couldn't register callback -> ignore presss */
                    button->timespressed = 0;
                }
            }
        }
    }
}

void button_init(button_t *button, unsigned char volatile near *portdd, unsigned char volatile near *port, int bit) {
    /* set data direction in */
    *portdd |= bit;

    button->port = port;
    button->bit = bit;
    button->single_press_callback = NULL;
    button->double_press_callback = NULL;
    button->isdepressed = 0;
    button->lastpressed = 0;
    button->timespressed = 0;

    timer_add_callback(_button_check_press, button, BUTTON_CHECK_INTERVAL, 0);
}

void button_on_single_press(button_t *button, void (*callback)(void *), void *data) {
    button->single_press_data = data;
    button->single_press_callback = callback;
}

void button_on_double_press(button_t *button, void (*callback)(void *), void *data) {
    button->double_press_data = data;
    button->double_press_callback = callback;
}