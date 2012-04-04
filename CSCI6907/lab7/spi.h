/*
 * spi.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120402
 * Lab:    5
 */

#ifndef SPI_H
#define SPI_H

/*
 * Convenience functions for working with single-slave SPI
 */

void spi_init(int hz);
void spi_set_bps(int bps);
void spi_set_mode(int mode);
void spi_begin();
unsigned char spi_transfer(unsigned char b);
void spi_end();

#endif