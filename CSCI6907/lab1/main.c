/*
 * main.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120220
 * Lab:    1
 *
 * Description: This lab displays strings of ASCII characters on the LED array
 * built into the ZNEO dev board.  Strings longer than the number of LEDs will
 * scroll either one LED at a time or one column at a time as chosen at
 * runtime.  A timer generates an interrupt every millisecond that drives the
 * entire operation of the program through a simple event-driven callback
 * model.  Each LED is represented by an object that registers itself with the
 * timer to draw a single row every two milliseconds.  The LEDs are tied
 * together by the ledarray class, which maps strings of characters to
 * individual LEDs and defines the behavior of inter-LED activity such as
 * scrolling, which is also driven by callbacks registered with the timer.
 * A button class is defined to capture single and double presses, driven by
 * the timer interrupt.  The button class is instatiated three times in the
 * main function for each of the three hardware buttons on the ZNEO dev board.
 * The main function registers callbacks with each button event to display
 * different strings on the LED array.
 *
 * Other files: button.{c,h} led.{c,h} ledarray.{c,h} matrix.h timer.{c,h}
 * Problems:    none
 *
 * Enhancements: I implemented "smooth scrolling" to shift characters from
 * right to left one column at a time.  This can be turned on and off at
 * runtime.  I purposely let characters appear to scroll through the empty
 * space between the LED panels which gives it a smoother look.  Double press
 * button 3 to see the effect at slower speed.  I also took care to ensure
 * the string could be updated even if it is shifted halfway between LED
 * panels.  Double press button 1 (or button 2 if you wait 255 seconds) to see
 * that effect.
 *
 * Questions:
 * 1. Did you order the display characters by column or row?  Does it matter?
 * 
 * I ordered scanned the pixels across the LED array by rows.  I did it that
 * way because the supplied matrix.h was populated with character data in rows.
 * It probably doesn't matter either way, except that you could extract more
 * efficiency by scanning column-wise, as a column could still fit in the byte
 * sized character data and there are fewer columns than rows.
 *
 * 2. Did you find that yoru display blinked or flickered, or that the
 * characters seemed dim, or that one row or column of dots was brighter than
 * the rest?  If you solved thath, what was the problem?  If not, what do you
 * think the problem is?
 *
 * When I was first developing the lab, I did not know about timers, so I
 * scanned the LEDs in a tight loop.  This gave each individual LED a
 * persistent, uniform look and brightness.  When I changed the program to be
 * timer driven, it took some fine tuning to find the correct refresh interval
 * the LEDs didn't flicker.  I found that about 2 ms per row is just about
 * right.
 *
 * 3. Briefly describe how did you de-bounce the switches?
 *
 * Basically, I ignore anything that happens for 20 ms after a valid button
 * event.  So when I first push a button, I register it as valid and don't
 * bother checking anything else about it for 20 ms.  Then when I release the
 * button, again, I ignore everything for 20 ms.  I've found this to be
 * highly effective at eliminating button bounces without affecting
 * interactivity.
 */

#include <zneo.h>
#include <stdlib.h>
#include "timer.h"
#include "ledarray.h"
#include "button.h"

#define PI 3.14159265

static button_t button1;
static button_t button2;
static button_t button3;

static timer_callback_t *updater_callback = NULL;

void _invalidate_updater_callback() {
    if (updater_callback) {
        timer_invalidate_callback(updater_callback);
        updater_callback = NULL;
    }
}

void _reset_ledarray() {
    _invalidate_updater_callback();
    ledarray_set_smooth_scroll(1);
    ledarray_set_scroll_rate(50);
    ledarray_set_restart_scroll_on_update(1);
}

void _update_led_uptime_seconds(void *data) {
    ledarray_setdecimal(timer_get_uptime());
}

void _update_led_uptime_seconds_hex(void *data) {
    ledarray_sethexadecimal(timer_get_uptime());
}

void _update_led_uptime_millis(void *data) {
    ledarray_setdecimal(timer_get_uptime_millis());
}

void _on_button1_single_press(void *data) {
    _reset_ledarray();
    _update_led_uptime_seconds(NULL);
    ledarray_set_restart_scroll_on_update(0);
    updater_callback = timer_add_callback(_update_led_uptime_seconds, NULL, 1000, 0);
}

void _on_button1_double_press(void *data) {
    _reset_ledarray();
    _update_led_uptime_millis(NULL);
    ledarray_set_restart_scroll_on_update(0);
    updater_callback = timer_add_callback(_update_led_uptime_millis, NULL, 1, 0);  
}

void _on_button2_single_press(void *data) {
    _reset_ledarray();
    ledarray_setdouble(PI);
}

void _on_button2_double_press(void *data) {
    _reset_ledarray();
    _update_led_uptime_seconds_hex(NULL);
    ledarray_set_restart_scroll_on_update(0);
    updater_callback = timer_add_callback(_update_led_uptime_seconds_hex, NULL, 1000, 0);
}

void _on_button3_single_press(void *data) {
    _reset_ledarray();
    ledarray_set_smooth_scroll(0);
    ledarray_setstring("Hello World from ZNEO");
}

void _on_button3_double_press(void *data) {
    _reset_ledarray();
    ledarray_set_scroll_rate(100);
    ledarray_setstring("James Lee CSCI 6907");
}

void _set_oscillator() {
    /*
     * SAMPLE CODE FROM INSTRUCTOR
     *
     * Good enough for selecting the external clock
     * TODO: Figure out how to use SYS_CLK_SRC, SYS_CLK_FREQ
     */

    // unlock the oscillator control register
    OSCCTL = 0xE7; OSCCTL = 0x18;  
   
    // follow with ONE of these TWO settings

    // A0 = 1010_0000 = internal 5.52 MHz
    //OSCCTL = 0xA0;                    

    // 61 = 0110_0001 = external 18.432 Hz
    OSCCTL = 0x61;
}

void main() {
    _set_oscillator();
    timer_init(18432000);
    
    ledarray_init();

    /* perform default behavior of displaying "Hello World from ZNEO" */
    _on_button3_single_press(NULL);

    /* initialize the three physical buttons */
    button_init(&button1, &PDDD, &PDIN, (1 << 3));
    button_init(&button2, &PFDD, &PFIN, (1 << 6));
    button_init(&button3, &PFDD, &PFIN, (1 << 7));

    /* register callbacks */
    button_on_single_press(&button1, _on_button1_single_press, NULL);
    button_on_double_press(&button1, _on_button1_double_press, NULL);
    button_on_single_press(&button2, _on_button2_single_press, NULL);
    button_on_double_press(&button2, _on_button2_double_press, NULL);
    button_on_single_press(&button3, _on_button3_single_press, NULL);
    button_on_double_press(&button3, _on_button3_double_press, NULL);

    while (1) {
        // noop
    }
}
