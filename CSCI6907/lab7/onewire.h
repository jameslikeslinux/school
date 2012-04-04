/*
 * onewire.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120403
 * Lab:    7
 */

#ifndef ONEWIRE_H
#define ONEWIRE_H

/* initialize PA0 1wire communication */
void onewire_init();

/* call before any 1wire operation */
void onewire_start();

/* call after every 1wire operation */
void onewire_stop();

/* initiate a 1wire command */
int onewire_reset();

/* write 'b' to 1wire bus */
void onewire_transmit(unsigned char b);

/* receive a byte from the 1wire bus */
unsigned char onewire_receive();

#endif