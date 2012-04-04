/*
 * onewire_mem.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120403
 * Lab:    7
 */

#ifndef ONEWIRE_MEM_H
#define ONEWIRE_MEM_H

#define ONEWIRE_MEM_MAX_ADDR 0x1ff

void onewire_mem_init();
unsigned char onewire_mem_read(unsigned short addr);
void onewire_mem_write(unsigned short addr, unsigned char b);

#endif