/*
 * unio_mem.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120403
 * Lab:    7
 */

#include "unio.h"
#include "unio_mem.h"
#include "timer.h"

#define UNIO_MEM_ADDR 0xa0  /* specific for 11AA family */

/* COMMANDS */
#define UNIO_MEM_READ 0x03
#define UNIO_MEM_WREN 0x96
#define UNIO_MEM_WRITE 0x6C

void unio_mem_init() {
    /* chip can operate at 10-100 KHz */
    /* -> choose 20 KHz for balance of speed and stability */
    /* (it also fits nicely in a time division on an oscilloscope) */
    unio_set_bitperiod(50);
}

unsigned char unio_mem_read(unsigned short addr) {
    unsigned char b;

    unio_start();
    unio_header();

    if (!unio_address(UNIO_MEM_ADDR)) {
        goto FAIL;
    }

    if (!unio_transmit(UNIO_MEM_READ, 1)) {
        goto FAIL;
    }

    if (!unio_transmit(addr >> 8, 1)) {
        goto FAIL;
    }

    if (!unio_transmit(addr, 1)) {
        goto FAIL;
    }

    if (!unio_receive(&b, 0)) {
        goto FAIL;
    }

    unio_stop();
    return b;

FAIL:
    /* if we don't get slave ack, abort the operation */
    /* -> put bus into standby */
    unio_standby();
    unio_stop();
    return 0;
}

void unio_mem_write(unsigned short addr, unsigned char b) {
    unio_start();
    unio_header();

    /* ENABLE WRITES */
    if (!unio_address(UNIO_MEM_ADDR)) {
        goto FAIL;
    }

    if (!unio_transmit(UNIO_MEM_WREN, 0)) {
        goto FAIL;
    }

    /* ISSUE WRITE COMMAND */
    unio_header();

    if (!unio_address(UNIO_MEM_ADDR)) {
        goto FAIL;
    }

    if (!unio_transmit(UNIO_MEM_WRITE, 1)) {
        goto FAIL;
    }

    if (!unio_transmit(addr >> 8, 1)) {
        goto FAIL;
    }

    if (!unio_transmit(addr, 1)) {
        goto FAIL;
    }

    if (!unio_transmit(b, 0)) {
        goto FAIL;
    }

    unio_stop();

    /* write cycle takes 5 ms max */
    timer_sleep(5);

    return;

FAIL:
    unio_standby();
    unio_stop();
}