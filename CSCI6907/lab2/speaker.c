/*
 * speaker.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120228
 * Lab:    2
 */

#include <zneo.h>
#include "speaker.h"

#define TIMER_ENABLE (1 << 7)
#define TIMER_MODE 1        /* continuous */
#define TIMER_PRESCALE 1    /* smallest timeout at 18 MHz => (65532 * 1)/18432000 = 3.556 ms */

static int hz;

void speaker_init(int h) {
    hz = h;

    /* disable and reset timer */
    T1CTL = 0;

    /* set timer type mode */
    T1CTL0 |= ((TIMER_MODE >> 3) << 7);
    T1CTL1 |= ((TIMER_MODE & 0x7) << 0);

    /* set prescaler */
    T1CTL1 |= ((TIMER_PRESCALE & 0x7) << 3);

    /* set PC0 as input */
    PCDD |= (1 << 0);

    /* set PC1 as output */
    PCDD &= ~(1 << 1);

    /* set PC0 as T1IN */
    PCAFH &= ~(1 << 0);
    PCAFL |= (1 << 0);

    /* set PC1 as TIOUT */
    PCAFH &= ~(1 << 1);
    PCAFL |= (1 << 1);
}

void speaker_play(int frequency) {
    /* start counter at 0 */
    T1HL = 0;
    
    /* set reload value */
    T1R = hz / (frequency * 4 * TIMER_PRESCALE);

    /* enable timer */
    T1CTL |= TIMER_ENABLE;
}

void speaker_stop() {
    T1CTL &= ~TIMER_ENABLE;
}