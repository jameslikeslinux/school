/*
 * spi_mem.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120402
 * Lab:    5
 */

#ifndef SPI_MEM_H
#define SPI_MEM_H

#define SPI_MEM_MAX_ADDR 0x1ff

void spi_mem_init();
unsigned char spi_mem_read(unsigned short addr);
void spi_mem_write(unsigned short addr, unsigned char b);

#endif