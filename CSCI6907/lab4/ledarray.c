/*
 * ledarray.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120220
 * Lab:    1
 */

#include <zneo.h>
#include <string.h>
#include <stdio.h>
#include <float.h>
#include "led.h"
#include "timer.h"

#define NUM_LEDS 5      /* define an extra dummy LED for staging characters */
#define STR_MAXLEN 256

static led_t leds[NUM_LEDS];

/* the string currently being displayed */
static char str[STR_MAXLEN] = "";

/* the length of the currently displayed string */
static int strlength = 0;

/* index into the string of the character being displayed on the last LED */
static int index = 1;

/* for smooth scrolling...how many columns offset to the left we are */
static int pixel = 0;

static int /* bool */ smooth = 1;
static int /* bool */ restart_scroll_on_update = 1;

static timer_callback_t *timer_callback;

static volatile int scrolled_once;

/*
 * Advances the string over one whole LED panel
 */ 
void _ledarray_advance() {
    int i, j;
    
    /* display spaces on left LEDs if string is off to the right */
    for (i = 0; i < NUM_LEDS - (index + 1); i++) {
        led_set_char(&leds[i], ' ');
    }
    
    /* display characters until we run out of LEDs or the string ends */
    for (j = index - (NUM_LEDS - i - 1); i < NUM_LEDS && j < strlength; i++, j++) {
        led_set_char(&leds[i], str[j]);
    }
    
    if (i == 0) {   /* the string moved off completely to the left */
        index = 0;
        scrolled_once = 1;
        
        /* so just show empty LEDs */
        for (; i < NUM_LEDS - 1; i++) {
            led_set_char(&leds[i], ' ');
        }
        
        /* and stage the first character on the dummy LED for rescrolling */
        led_set_char(&leds[NUM_LEDS - 1], str[0]);
    } else {
        /* otherwise just pad any necessary LEDs to the right with spaces */
        for (; i < NUM_LEDS; i++) {
            led_set_char(&leds[i], ' ');
        }
    }
    
    index++;
}

/*
 * Shifts pixels one column to the left for smooth scrolling
 */
void _ledarray_advance_pixel() {
    int i;
    
    for (i = 0; i < NUM_LEDS - 1; i++) {
        led_shift(&leds[i], &leds[i + 1]);
    }
    
    led_shift(&leds[NUM_LEDS - 1], NULL);
}

/*
 * Timer callback; scroll string
 */
void _ledarray_scroll(void *data) {
    /* only scroll if necessary */
    if (strlength < NUM_LEDS) {
        return;
    }
    
    if (pixel++ == LED_NUM_COLS - 1) {
        /* 
         * we still want to use the normal scrolling function every (6th) time
         * even if we are smooth scrolling so the dummy LED gets updated
         */ 
        _ledarray_advance();
        pixel = 0;
    } else if (smooth) {
        _ledarray_advance_pixel();
    }
}

void ledarray_init() {
    /* define real LEDs */
    led_init(&leds[0], &PEOUT, (1 << 7), ' ');
    led_init(&leds[1], &PGOUT, (1 << 7), ' ');
    led_init(&leds[2], &PEOUT, (1 << 5), ' ');
    led_init(&leds[3], &PEOUT, (1 << 6), ' ');

    /*
     * This dummy LED serves as a way to stage characters off to the right
     * so that data can be shifted from it as used by smooth scrolling
     */
    led_init(&leds[4], NULL, 0, ' ');
    
    smooth = 1;
    
    /* define a callback to fire every 100 ms to scroll the panels */
    timer_callback = timer_add_callback(_ledarray_scroll, NULL, 100, 0);
}

void ledarray_set_scroll_rate(int millies_per_char) {
    timer_change_callback_interval(timer_callback, millies_per_char);
}

void ledarray_set_smooth_scroll(int s) {
    pixel = 0;
    smooth = s;
}

void ledarray_set_restart_scroll_on_update(int r) {
    restart_scroll_on_update = r;
}

void ledarray_setstring(char *newstr) {
    int i;
    int newstrlength;

    /* copy the string over; truncate at STR_MAXLEN chars (including null) */
    memcpy(str, newstr, STR_MAXLEN);

    newstrlength = strlen(newstr);  
    if (newstrlength >= STR_MAXLEN) {
        str[STR_MAXLEN - 1] = '\0';
    }
    
    strlength = newstrlength;
    
    if (strlength < NUM_LEDS) {
        /*
         * if the string is less than the number of LEDs, then all we have to
         * do is display them
         */
        for (i = 0; i < strlength; i++) {
            led_set_char(&leds[i], str[i]);
        }
        
        for (; i < NUM_LEDS; i++) {
            led_set_char(&leds[i], ' ');
        }
    } else if (restart_scroll_on_update) {
        /*
         * otherwise, if we're restarting the scroll, then reset the scroll
         * related variables and advance one LED to essentially reinitialize
         * everything
         */ 
        index = 0;
        pixel = 0;
        _ledarray_advance();
    } else {
        /*
         * otherwise, we have to take care to keep the old string's position
         * on the LEDs, so set the string and "redo" the last scroll operation
         * to get everything looking the way it should
         */
        index--;
        _ledarray_advance();
        
        if (smooth) {
            for (i = 0; i < pixel; i++) {
                _ledarray_advance_pixel();
            }
        }
    }
}

void ledarray_setstring1(char *newstr, int scroll, int wait) {
    char str[NUM_LEDS];

    if (scroll) {
        ledarray_setstring(newstr);
        
        if (wait) {
            if (strlen(newstr) > NUM_LEDS - 1) {
                scrolled_once = 0;
                while (!scrolled_once);
            } else {
                timer_sleep(3000);
            }
        }  
    } else {
        /* truncate string and display it */
        strncpy(str, newstr, NUM_LEDS - 1);
        str[NUM_LEDS - 1] = '\0';
        ledarray_setstring(str);
    }
}

void ledarray_setdecimal(int number) {
    char numstr[12];    /* "-2147483647" */
    sprintf(numstr, "%4d", number);
    ledarray_setstring(numstr);
}

void ledarray_sethexadecimal(int number) {
    char numstr[12];    /* "-0xffffffff" */
    sprintf(numstr, "%#4x", number);
    ledarray_setstring(numstr);
}

void ledarray_setdouble(double number) {
    /* http://stackoverflow.com/questions/1701055/what-is-the-maximum-length-in-chars-needed-to-represent-any-double-value */
    char numstr[DBL_MANT_DIG - DBL_MIN_EXP + 4];
    sprintf(numstr, "%4.7g", number);
    ledarray_setstring(numstr);
}

void ledarray_stopwaiting() {
    scrolled_once = 1;
}
