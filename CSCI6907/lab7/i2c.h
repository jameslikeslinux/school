/*
 * i2c.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120402
 * Lab:    5
 */

#ifndef I2C_H
#define I2C_H

/*
 * Convenience functions for working with ZNEO I2C controller
 */

void i2c_init(int hz);
void i2c_set_bps(int bps);
void i2c_start();
void i2c_stop();
void i2c_set_data(unsigned char b);
void i2c_nak();
unsigned char i2c_get_data();
int i2c_get_ack();

#endif