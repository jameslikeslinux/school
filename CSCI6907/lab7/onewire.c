/*
 * onewire.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120403
 * Lab:    7
 */

#include <zneo.h>
#include "onewire.h"
#include "utils.h"

#define ONEWIRE_PIN (1 << 0)

static unsigned short istat;

void _onewire_write_0() {
    /* drive bus low and delay 60 us */
    PAOUT &= ~ONEWIRE_PIN;
    delay_micros(/* 18.432 * 60 us = */ 1106);

    /* release bus and delay 10 us */
    PAOUT |= ONEWIRE_PIN;
    delay_micros(/* 18.432 * 10 us = */ 184);
}

void _onewire_write_1() {
    /* drive bus low and delay 6 us */
    PAOUT &= ~ONEWIRE_PIN;
    delay_micros(/* 18.432 * 6 us = */ 111);

    /* release bus and delay 64 us */
    PAOUT |= ONEWIRE_PIN;
    delay_micros(/* 18.432 * 64 us = */ 1180);
}

int _onewire_read() {
    int bit;

    /* drive bus low and delay 6 us */
    PAOUT &= ~ONEWIRE_PIN;
    delay_micros(/* 18.432 * 6 us = */ 111);

    /* release bus and delay 9 us */
    PAOUT |= ONEWIRE_PIN;
    delay_micros(/* 18.432 * 9 us = */ 166);

    bit = PAIN & ONEWIRE_PIN;

    delay_micros(/* 18.432 * 55 us = */ 1014);

    return bit;
}

void onewire_init() {
    /* set PA1 as output */
    PADD &= ~ONEWIRE_PIN;

    /* enable open drain */
    PAOC |= ONEWIRE_PIN;

    /* release the bus */
    PAOUT |= ONEWIRE_PIN;
}

void onewire_start() {
    istat = TDI();
}

void onewire_stop() {
    RI(istat);
}

int onewire_reset() {
    int present;

    /* drive bus low and delay 480 us */
    PAOUT &= ~ONEWIRE_PIN;
    delay_micros(/* 18.432 * 480 us = */ 8848);

    /* release bus and delay 70 us */
    PAOUT |= ONEWIRE_PIN;
    delay_micros(/* 18.432 * 70 us = */ 1290);

    /* sample bus, 0 = device present; 1 = no device present */
    present = !(PAIN & ONEWIRE_PIN);
    delay_micros(/* 18.432 * 410 us = */ 7557);

    return present;
}

void onewire_transmit(unsigned char b) {
    int i;

    /* transmit LSB first */
    for (i = 0; i <= 7; i++) {
        if (b & (1 << i)) {
            _onewire_write_1();
        } else {
            _onewire_write_0();
        }
    }
}

unsigned char onewire_receive() {
    int i;
    unsigned char b;

    b = 0;

    /* receive LSB first */
    for (i = 0; i <= 7; i++) {
        if (_onewire_read()) {
            b |= (1 << i);
        } else {
            b &= ~(1 << i);
        }
    }

    return b;
} 