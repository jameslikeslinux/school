/*
 * unio.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120403
 * Lab:    7
 */

#ifndef UNIO_H
#define UNIO_H

/* initializes PA1 for UNI/O communication */
void unio_init();
void unio_set_bitperiod(int bitperiod);	/* in microseconds */
void unio_standby();

/* call before any UNI/O operation */
void unio_start();

/* call after every UNI/O operation */
void unio_stop();

/* start new command; synchronizes slaves */
void unio_header();

/* select device; returns true if device responded */
int unio_address(unsigned char addr);

/* write 'b' to bus; 'mak' false to end command */
/* returns true if transmit success */
int unio_transmit(unsigned char b, int mak);

/* receives byte 'b' from bus; 'mak' false to end command */
/* returns true if receive success */
int unio_receive(unsigned char *b, int mak);

#endif