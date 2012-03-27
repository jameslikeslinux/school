/*
 * keyboard.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120321
 * Lab:    4
 */

#include <zneo.h>
#include <string.h>
#include <stdio.h>
#include "keyboard.h"
#include "speaker.h"
#include "utils.h"
#include "ledarray.h"

void keyboard_init(keyboard_t *this, unsigned volatile char near *in, unsigned volatile char near *out, unsigned volatile char near *dd, unsigned volatile char near *oc, unsigned char clock_bit, unsigned char data_bit) {
    this->in = in;
    this->out = out;
    this->clock_bit = clock_bit;
    this->data_bit = data_bit;

    /* set ports as outputs */
    *dd &= ~(1 << clock_bit);
    *dd &= ~(1 << data_bit);

    /* enable open-collector mode */
    *oc |= (1 << clock_bit);
    *oc |= (1 << data_bit);

    /* release the bus */
    *out |= (1 << clock_bit);
    *out |= (1 << data_bit);

    /* set frequency for playing ticks */
    speaker_set(1000);
}

unsigned short _keyboard_uninhibit(keyboard_t *this) {
    unsigned short istat;

    /* disable interrupts */
    istat = TDI();

    /* release the bus */
    *this->out |= (1 << this->clock_bit);

    return istat;
}

void _keyboard_inhibit(keyboard_t *this, unsigned short istat) {
    /* grab the bus */
    *this->out &= ~(1 << this->clock_bit);

    /* reenable interrupts */
    RI(istat);

    /* wait the minimum 100 us to abort any keyboard activity and silence the bus */
    delay_micros(100);
}

char keyboard_get_scancode(keyboard_t *this) {
    unsigned short istat;
    char data, code, parity;
    int i, code_started;

    this->stop = 0;

    /* loop until until recevied scancode */
    while (1) {
        code_started = 0;

        /*
         * loop until we find the start of a scancode
         * alternating every 100 us between checking the keyboard and enabling interrupts
         */
        do {
            istat = _keyboard_uninhibit(this);

            /* the fastest possible clock low is 30 us (half period of 16.7 KHz) */
            /* so if we check the clock at a rate faster than 30 us, we shouldn't miss it going low */
            /* after releasing the bus, check for scancode start every 5 us for 100 us */
            for (i = 0; i < 20 && !code_started; i++) {
                if (!((*this->in >> this->clock_bit) & 1)) {
                    code_started = 1;
                } else {
                    delay_micros(5);
                }
            }

            if (this->stop) {
                _keyboard_inhibit(this, istat);
                return 0;
            }
            
            if (!code_started) {
                /* if we didn't see a code start, grab the bus and let some interrupts happen */
                _keyboard_inhibit(this, istat);
            }
        } while (!code_started);

        /* check start bit */
        if ((*this->in >> this->data_bit) & 1) {
            /* start bit must be low */
            _keyboard_inhibit(this, istat);
            continue;
        }

        speaker_start();

        /* wait for data */
        while (!((*this->in >> this->clock_bit) & 1));    /* while clock low */
        while ((*this->in >> this->clock_bit) & 1);       /* while clock high */

        code = 0;
        parity = 0;

        for (i = 0; i < 9; i++) {
            data = (*this->in >> this->data_bit) & 1;
            code |= data << i;
            parity ^= data;
            while (!((*this->in >> this->clock_bit) & 1));    /* while clock low */
            while ((*this->in >> this->clock_bit) & 1);       /* while clock high */
        }

        speaker_stop();

        if (!parity) {
            /* single bit error detected */
            _keyboard_inhibit(this, istat);
            continue;
        }

        /* check stop bit */
        if (!((*this->in >> this->data_bit) & 1)) {
            /* stop bit must be high */
            _keyboard_inhibit(this, istat);
            continue;
        }
        
        _keyboard_inhibit(this, istat);
        ledarray_sethexadecimal(code);
        return code;
    }

    /* this shouldn't happen */
    return 0;
}

#define SECONDARY (1 << 8)
#define SHIFT (1 << 9)
#define CONTROL (1 << 10)

short keyboard_get_keypress(keyboard_t *this) {
    char scancode;

    /* reset keypress */
    this->keypress &= ~SECONDARY;
    this->keypress &= 0xff00;

    while (1) {
        scancode = keyboard_get_scancode(this);

        if (scancode == 0xf0) {
            /* handle break code */
            scancode = keyboard_get_scancode(this);

            if (scancode == 0x12 || scancode == 0x59) {
                /* left and right shift */
                this->keypress &= ~SHIFT;
            } else if (scancode == 0x14) {
                /* left ctrl */
                this->keypress &= ~CONTROL;
            }
        } else if (scancode == 0xe0) {
            scancode = keyboard_get_scancode(this);

            if (scancode == 0xf0) {
                /* handle secondary break code */
                scancode = keyboard_get_scancode(this);

                if (scancode == 0x14) {
                    /* right ctrl */
                    this->keypress &= ~CONTROL;
                }
            } else if (scancode == 0x14) {
                /* right ctrl */
                this->keypress |= CONTROL;
            } else {
                this->keypress |= SECONDARY;

                /* set scancode and return */
                this->keypress |= scancode;
                return this->keypress;
            }
        } else if (scancode == 0x12 || scancode == 0x59) {
            /* left and right shift */
            this->keypress |= SHIFT;
        } else if (scancode == 0x14) {
            /* left ctrl */
            this->keypress |= CONTROL;
        } else {
            /* set scancode and return */
            this->keypress |= scancode;
            return this->keypress;
        }
    }
}

#define KEYBOARD_NUM_KEYS 50
static rom char keycode[KEYBOARD_NUM_KEYS] = {0x1c, 0x32, 0x21, 0x23, 0x24, 0x2b, 0x34, 0x33, 0x43, 0x3b, 0x42, 0x4b, 0x3a, 0x31, 0x44, 0x4d, 0x15, 0x2d, 0x1b, 0x2c, 0x3c, 0x2a, 0x1d, 0x22, 0x35, 0x1a, 0x45, 0x16, 0x1e, 0x26, 0x25, 0x2e, 0x36, 0x3d, 0x3e, 0x46, 0x0e, 0x4e, 0x55, 0x5d, 0x66, 0x29, 0x76, 0x54, 0x5b, 0x4c, 0x52, 0x41, 0x49, 0x4a};
static rom char lowercase[KEYBOARD_NUM_KEYS] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '`', '-', '=', '\\', 127, ' ', 27, '[', ']', ';', '\'', ',', '.', '/'};
static rom char uppercase[KEYBOARD_NUM_KEYS] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ')', '!', '@', '#', '$', '%', '^', '&', '*', '(', '~', '_', '+', '|', 127, ' ', 27, '{', '}', ':', '"', '<', '>', '?'};

char * keyboard_get_string(keyboard_t *this) {
    static char str[16];
    char key;
    short kp;
    int i;

    kp = keyboard_get_keypress(this);

    if (kp & SECONDARY) {
        /* don't print secondary characters for now */
        return "";
    }

    key = 0;

    for (i = 0; i < KEYBOARD_NUM_KEYS; i++) {
        if (((char) kp) == keycode[i]) {
            if (kp & SHIFT) {
                key = uppercase[i];
            } else {
                key = lowercase[i];
            }
        }
    }

    if (key) {
        if (kp & CONTROL) {
            sprintf(str, "<CTRL+%c>", key);
        } else {
            sprintf(str, "%c", key);
        }

        return str;
    }

    return "";
}

void keyboard_stop(keyboard_t *this) {
    this->stop = 1;
}