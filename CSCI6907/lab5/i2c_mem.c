/*
 * i2c_mem.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120402
 * Lab:    5
 */

#include "i2c.h"
#include "i2c_mem.h"

#define I2C_MEM_CTL 0xa0
#define I2C_MEM_READ (1 << 0)

void i2c_mem_init() {
    /* set standard i2c bit rate (see chip spec) */
    i2c_set_bps(100000);
}

unsigned char i2c_mem_read(unsigned short addr) {
    unsigned char b;

    /* SET ADDRESS */
    i2c_start();
    i2c_set_data(I2C_MEM_CTL | (((addr >> 8) & 0x7) << 1));
    if (!i2c_get_ack()) {
        goto FAIL;
    }
    i2c_set_data(addr);
    if (!i2c_get_ack()) {
        goto FAIL;
    }

    /* READ FROM ADDRESS */
    i2c_start();
    i2c_nak();  /* only reading one byte, so don't acknowledge receive */
    i2c_set_data(I2C_MEM_CTL | (((addr >> 8) & 0x7) << 1) | I2C_MEM_READ);
    if (!i2c_get_ack()) {
        goto FAIL;
    }
    b = i2c_get_data();
    i2c_stop();

    return b;

FAIL:
    i2c_stop();
    return 0;
}

void i2c_mem_write(unsigned short addr, unsigned char b) {
    int wip;

    /* send write instruction, address, and byte */
    i2c_start();
    i2c_set_data(I2C_MEM_CTL | (((addr >> 8) & 0x7) << 1));
    if (!i2c_get_ack()) {
        goto FAIL;
    }
    i2c_set_data(addr);
    if (!i2c_get_ack()) {
        goto FAIL;
    }
    i2c_set_data(b);
    if (!i2c_get_ack()) {
        goto FAIL;
    }

    /* initiate write cycle by sending stop condition */
    i2c_stop();

    /*
     * wait until device acknowledges again
     * indicating write cycle finished
     */
    wip = 1;
    while (wip) {
        i2c_start();
        i2c_set_data(I2C_MEM_CTL | (((addr >> 8) & 0x7) << 1));
        if (i2c_get_ack()) {
            wip = 0;
        }
    }

    i2c_stop();
    return;

FAIL:
    i2c_stop();
}