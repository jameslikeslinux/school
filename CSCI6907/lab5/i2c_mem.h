/*
 * i2c_mem.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120402
 * Lab:    5
 */

#ifndef I2C_MEM
#define I2C_MEM

#define I2C_MEM_MAX_ADDR 0x7ff

void i2c_mem_init();
unsigned char i2c_mem_read(unsigned short addr);
void i2c_mem_write(unsigned short addr, unsigned char b);

#endif