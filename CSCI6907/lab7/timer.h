/*
 * timer.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120220
 * Lab:    1
 */

#ifndef TIMER_H
#define TIMER_H

typedef struct {
    /*
     * PRIVATE
     */
    void (*func)(void *);   /* the callback function pointer */
    void *data;     /* data to be passed to the callback function */
    int millis;     /* to reset 'trigger' if 'isoneshot' is false */
    int trigger;    /* counts down from 'millis'; callback fires at 0 */
    int isvalid;
    int isoneshot;
} timer_callback_t;

/*
 * Initialize the system timer to fire every 1 ms
 *
 * hz - The system clock
 */
void timer_init(int hz);

/*
 * Add a callback to be called every 'millis' ms
 * or just once 'millis' ms in the future if 'isoneshot' is set
 */
timer_callback_t * timer_add_callback(void (*func)(void *), void *data, int millis, int isoneshot);

/*
 * Change the callback interval for (non-oneshot) callbacks
 *
 * callback - as returned from 'timer_add_callback'
 */
void timer_change_callback_interval(timer_callback_t *callback, int millis);

/*
 * Cancel a callback, freeing up room for more callbacks
 *
 * callback - as returned from 'timer_add_callback'
 */
void timer_invalidate_callback(timer_callback_t *callback);


/*
 * Get time since microcontroller start
 */
unsigned long timer_get_uptime();
unsigned long timer_get_uptime_millis();

/*
 * Naive sleep function; spins for 'millis' ms
 */
void timer_sleep(int millis);

/*
 * Wakeup any sleepers
 */
void timer_wakeup();

#endif
