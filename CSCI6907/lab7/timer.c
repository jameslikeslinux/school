/*
 * timer.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120220
 * Lab:    1
 */

#include <zneo.h>
#include <stdlib.h>
#include "timer.h"
#include "led.h"

#define MAX_CALLBACKS 12

static timer_callback_t timer_callbacks[MAX_CALLBACKS];
static int num_callbacks = 0;
static volatile unsigned long uptime = 0;
static volatile int wake = 0;

void interrupt timer_isr() {
    int i;
    
    uptime++;
    
    /*
     * try and update every callback;
     * fire valid callback when trigger reaches zero
     */
    for (i = 0; i < MAX_CALLBACKS; i++) {
        if (timer_callbacks[i].isvalid && --timer_callbacks[i].trigger == 0) {

            /* callback fired */

            timer_callbacks[i].trigger = timer_callbacks[i].millis;
            timer_callbacks[i].func(timer_callbacks[i].data);

            if (timer_callbacks[i].isoneshot) {
                timer_callbacks[i].isvalid = 0;
            }
        }
    }
}

void timer_init(int hz) {
    int i;

    for (i = 0; i < MAX_CALLBACKS; i++) {
        timer_callbacks[i].isvalid = 0;
    }


    /*
     * SAMPLE CODE FROM INSTRUCTOR
     *
     * Good enough for generating an interrupt every 1 ms
     */

    SET_VECTOR(TIMER0,timer_isr); // Assign interrupt vector 
    EI();
    
    // Disable timer (why?)
    T0CTL1 |= 0x7f;         
    
    // Set timer0 control 0 and 1 
    T0CTL1 = 0x39;                // 0011_1001 0=disable
                                  //   0=tpol, 111=prescale, 001=mode
    T0CTL0 = 0x00;                // 0=mode, 00=ticonfig, 0=cascade, 
                                  //   000=pwmd, 0=incap

    // Initial counter value
    T0L = 0x01;                   
    T0H = 0x00;

    // Set timer reload value
    //   reload = clock / prescale * timeout  
    //   desired timeout is 0.001 seconds
    T0R = hz / 128 * 0.001;

    // Enable Timer0 interrupt (nominal priority)
    //IRQ0ENL |= 0x20;
    IRQ0ENH |= 0x20;

    // Configure the Output pin (so we can see it on scope)
    //PADD &= ~0x02;
    //PAAF |= 0x02;

    // Enable Timer0
    T0CTL1 |= 0x80;
}

timer_callback_t * timer_add_callback(void (*func)(void *), void *data, int millis, int isoneshot) {
    int i;
    timer_callback_t *callback = NULL;

    /* try to find available callback slot */
    for (i = 0; i < MAX_CALLBACKS; i++) {
        if (!timer_callbacks[i].isvalid) {
            callback = &timer_callbacks[i];
            break;
        }
    }

    if (!callback) {
        /* oh, no... we didn't find one */
        return NULL;
    }

    /* set callback data */
    callback->func = func;
    callback->data = data;
    callback->millis = millis;
    callback->trigger = millis;
    callback->isoneshot = isoneshot;

    /* finally, validate it */
    callback->isvalid = 1;

    return callback;
}

void timer_change_callback_interval(timer_callback_t *callback, int millis) {
    callback->millis = millis;
    callback->trigger = millis;
}

void timer_invalidate_callback(timer_callback_t *callback) {
    callback->isvalid = 0;
}

unsigned long timer_get_uptime() {
    return uptime / 1000;
}

unsigned long timer_get_uptime_millis() {
    return uptime;
}

void timer_sleep(int millis) {
    long start_uptime = uptime;
    wake = 0;
    
    while (start_uptime + millis > uptime && !wake) {
        // noop
    }
}

void timer_wakeup() {
    wake = 1;
}
