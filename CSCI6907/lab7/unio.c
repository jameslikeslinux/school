/*
 * unio.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120403
 * Lab:    7
 */

#include <zneo.h>
#include "unio.h"
#include "utils.h"

#define UNIO_PIN (1 << 1)

typedef enum {
    UNIO_ZERO,  /* read high-to-low transition */
    UNIO_ONE,   /* read low-to-high transition */
    UNIO_NONE   /* no transition detected */
} unio_value_t;

static unsigned short unio_bitdelay, unio_half_bitdelay, unio_istat;

void _unio_write_0() {
    /* high-to-low */
    PAOUT |= UNIO_PIN;
    delay_micros(unio_bitdelay);
    PAOUT &= ~UNIO_PIN;
    delay_micros(unio_bitdelay);
}

void _unio_write_1() {
    /* low-to-high */
    PAOUT &= ~UNIO_PIN;
    delay_micros(unio_bitdelay);
    PAOUT |= UNIO_PIN;
    delay_micros(unio_bitdelay);
}

unio_value_t _unio_read() {
    /* release the bus */
    PAOUT |= UNIO_PIN;

    /* try to read in middle of low or high period */
    delay_micros(unio_half_bitdelay);

    if (PAIN & UNIO_PIN) {
        /* high */
        delay_micros(unio_bitdelay);
        if (!(PAIN & UNIO_PIN)) {
            /* low */
            delay_micros(unio_half_bitdelay);
            return UNIO_ZERO;
        }
    } else {
        /* low */
        delay_micros(unio_bitdelay);
        if (PAIN & UNIO_PIN) {
            /* high */
            delay_micros(unio_half_bitdelay);
            return UNIO_ONE;
        }
    }

    return UNIO_NONE;
}

void unio_init() {
    /* set PA1 as output */
    PADD &= ~UNIO_PIN;

    /* enable internal pull-up */
    PAPUE |= UNIO_PIN;

    /* enable open drain */
    PAOC |= UNIO_PIN;

    /* set 10 KHz (100 us period) operation by default */
    unio_set_bitperiod(100);

    /* enter standby mode */
    unio_standby();
}

void unio_set_bitperiod(int bitperiod) {
    /* delay half of bitperiod */
    unio_bitdelay = 18.432 * (bitperiod / 2);

    /* set half of bit delay so I can read in the middle of a low or high */
    unio_half_bitdelay = unio_bitdelay >> 1;
}

void unio_standby() {
    /* drive bus high for at least 600 us */
    PAOUT |= UNIO_PIN;
    delay_micros(/* 18.432 * 600 us = */ 11059);
}

void unio_start() {
    unio_istat = TDI();
}

void unio_stop() {
    RI(unio_istat);
}

void unio_header() {
    /* drive bus low for at least 5 us */
    PAOUT &= ~UNIO_PIN;
    delay_micros(/* 18.432 * 5 us = */ 92);

    unio_transmit(0x55, 1);
}

int unio_address(unsigned char addr) {
    return unio_transmit(addr, 1);
}

int unio_transmit(unsigned char b, int mak) {
    int i;

    for (i = 7; i >= 0; i--) {
        if (b & (1 << i)) {
            _unio_write_1();
        } else {
            _unio_write_0();
        }
    }

    if (mak) {
        _unio_write_1();
    } else {
        _unio_write_0();
    }

    if (_unio_read() == UNIO_ONE) {
        return 1;
    } else {
        return 0;
    }
}

int unio_receive(unsigned char *b, int mak) {
    int i;
    unio_value_t read;

    for (i = 7; i >= 0; i--) {
        read = _unio_read();

        if (read == UNIO_ZERO) {
            *b &= ~(1 << i);
        } else if (read == UNIO_ONE) {
            *b |= (1 << i);
        } else {
            return 0;
        }
    }

    if (mak) {
        _unio_write_1();
    } else {
        _unio_write_0();
    }

    if (_unio_read() == UNIO_ONE) {
        return 1;
    } else {
        return 0;
    }    
}