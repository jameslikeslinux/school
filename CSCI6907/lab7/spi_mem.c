/*
 * spi_mem.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120402
 * Lab:    5
 */

#include "spi.h"
#include "spi_mem.h"

/* commands */
#define SPI_MEM_READ 3
#define SPI_MEM_WRITE 2
#define SPI_MEM_WRITE_ENABLE 6
#define SPI_MEM_READ_STATUS 5
#define SPI_MEM_STATUS_WIP (1 << 0)

void spi_mem_init() {
    spi_set_mode(0);
    spi_set_bps(2500000);
}

unsigned char spi_mem_read(unsigned short addr) {
    unsigned char b;

    spi_begin();
    spi_transfer(SPI_MEM_READ | ((addr >> 8) << 3));
    spi_transfer(addr);
    b = spi_transfer(0);
    spi_end();

    return b;
}

void spi_mem_write(unsigned short addr, unsigned char b) {
    int wip;
    
    /* enable write enable latch */
    spi_begin();
    spi_transfer(SPI_MEM_WRITE_ENABLE);
    spi_end();

    /* write the byte */
    spi_begin();
    spi_transfer(SPI_MEM_WRITE | ((addr >> 8) << 3));
    spi_transfer(addr);
    spi_transfer(b);
    spi_end();

    /* wait until write is complete */
    wip = 1;
    while (wip) {
        spi_begin();
        spi_transfer(SPI_MEM_READ_STATUS);
        b = spi_transfer(0);
        spi_end();
        wip = b & SPI_MEM_STATUS_WIP;
    }
}