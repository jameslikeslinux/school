/*
 * unio_mem.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120403
 * Lab:    7
 */

#ifndef UNIO_MEM_H
#define UNIO_MEM_H

#define UNIO_MEM_MAX_ADDR 0x7ff

void unio_mem_init();
unsigned char unio_mem_read(unsigned short addr);
void unio_mem_write(unsigned short addr, unsigned char b);

#endif