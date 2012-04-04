/*
 * spi.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120402
 * Lab:    5
 */

#include <zneo.h>
#include "spi.h"

#define SPI_MOSI (1 << 4)
#define SPI_MISO (1 << 5)
#define SPI_SCK (1 << 3)
#define SPI_SS (1 << 2)
#define SPI_REN (1 << 0)
#define SPI_TEN (1 << 6)
#define SPI_PHASE (1 << 4)
#define SPI_CPOL (1 << 3)
#define SPI_MASTER_MODE (1 << 1)
#define SPI_SS_OUTPUT (1 << 1)
#define SPI_SS_LOW (1 << 0)
#define SPI_TDRE (1 << 7)
#define SPI_RDRF (1 << 2)

static int spi_hz;

void spi_init(int hz) {
    spi_hz = hz;

    /* set alternate function on port C to SPI */
    PCAFL |= (SPI_MOSI | SPI_MISO | SPI_SCK | SPI_SS);

    ESPICTL |= SPI_MASTER_MODE;
    ESPIMODE |= SPI_SS_OUTPUT;

    /* set SPI defaults */
    spi_set_bps(hz);
    spi_set_mode(0);

    /* enable SPI */
    ESPICTL |= (SPI_TEN | SPI_REN);
}

void spi_set_bps(int bps) {
    /* BRG = HZ / (2 * BPS) */
    /* add 1 as a simple way of rounding up */
    ESPIBR = (spi_hz / (bps << 1)) + 1;
}

void spi_set_mode(int mode) {
    switch (mode) {
        case 0:
            ESPICTL &= ~SPI_CPOL;
            ESPICTL &= ~SPI_PHASE;
            break;

        case 1:
            ESPICTL &= ~SPI_CPOL;
            ESPICTL |= SPI_PHASE;
            break;

        case 2:
            ESPICTL |= SPI_CPOL;
            ESPICTL &= ~SPI_PHASE;
            break;

        case 3:
            ESPICTL |= SPI_CPOL;
            ESPICTL |= SPI_PHASE;
            break;
    }
}

void spi_begin() {
    /* enable slave */
    ESPITDCR |= SPI_SS_LOW;
}

unsigned char spi_transfer(unsigned char b) {
    /* wait for transmit register to empty, then transmit */
    while (!(ESPISTAT & SPI_TDRE));
    ESPIDATA = b;

    /* wait for receive register to fill, then read */
    while (!(ESPISTAT & SPI_RDRF));
    return ESPIDATA;
}

void spi_end() {
    /* disable slave */
    ESPITDCR &= ~SPI_SS_LOW;
}