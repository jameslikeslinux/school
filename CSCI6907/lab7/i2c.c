/*
 * i2c.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120402
 * Lab:    5
 */

#include <zneo.h>
#include "i2c.h"

#define I2C_SDA (1 << 7)
#define I2C_SCL (1 << 6)
#define I2C_ENABLE (1 << 7)
#define I2C_ENABLE_TDRE (1 << 3)
#define I2C_START (1 << 6)
#define I2C_STOP (1 << 5)
#define I2C_NAK (1 << 2)
#define I2C_TDRE (1 << 7)
#define I2C_RDRF (1 << 6)
#define I2C_ACK_VALID (1 << 7)
#define I2C_ACK (1 << 6)

static int i2c_hz;

void i2c_init(int hz) {
	i2c_hz = hz;

    /* set alternate function on port A to I2C */
    PAAFL |= (I2C_SDA | I2C_SCL);

    /* enable internal pull up on the I2C pins */
    PAPUE |= (I2C_SDA | I2C_SCL);

    /* set I2C defaults */
    i2c_set_bps(100000);

    /* enable I2C */
    I2CCTL |= (I2C_ENABLE | I2C_ENABLE_TDRE);
}

void i2c_set_bps(int bps) {
    /* BRG = HZ / (4 * BPS) */
    /* add 1 as a simple way of rounding up */
    I2CBR = (i2c_hz / (bps << 2)) + 1;
}

void i2c_start() {
    /* set send start condition */
    I2CCTL |= I2C_START;
}

void i2c_stop() {
    /* set send stop condition */
    I2CCTL |= I2C_STOP;
    while (I2CCTL & I2C_STOP);
}

void i2c_set_data(unsigned char b) {
    /* wait for transmit register to empty and write the byte */
    while (!(I2CISTAT & I2C_TDRE));
    I2CDATA = b;
    /* wait for transmit register to empty */
    while (!(I2CISTAT & I2C_TDRE));
}

void i2c_nak() {
    I2CCTL |= I2C_NAK;
}

unsigned char i2c_get_data() {
    /* wait for receive register to fill and return byte */
    while (!(I2CISTAT & I2C_RDRF));
    return I2CDATA;
}

int i2c_get_ack() {
    /* wait for acknowledgment to be valid and return the ack */
    while (!(I2CSTATE & I2C_ACK_VALID));
    return I2CSTATE & I2C_ACK;
}