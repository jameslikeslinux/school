/*
 * onewire_mem.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120403
 * Lab:    7
 */

#include "onewire.h"
#include "onewire_mem.h"
#include "timer.h"

/* COMMANDS */
#define ONEWIRE_MEM_SKIP_ROM 0xcc
#define ONEWIRE_MEM_READ 0xf0
#define ONEWIRE_MEM_WRITE_SCRATCHPAD 0x0f
#define ONEWIRE_MEM_READ_SCRATCHPAD 0xaa
#define ONEWIRE_MEM_COPY_SCRATCHPAD 0x55

void onewire_mem_init() {
    /* empty */
}

unsigned char onewire_mem_read(unsigned short addr) {
    unsigned char b;

    onewire_start();

    if (!onewire_reset()) {
        onewire_stop();
        return 0;
    }

    onewire_transmit(ONEWIRE_MEM_SKIP_ROM); /* only one device on bus */
    onewire_transmit(ONEWIRE_MEM_READ);
    onewire_transmit(addr);
    onewire_transmit(addr >> 8);
    b = onewire_receive();

    onewire_stop();
    
    return b;
}

void onewire_mem_write(unsigned short addr, unsigned char b) {
    unsigned char ta1, ta2, es;

    onewire_start();

    /*
     * WRITE DATA TO SCRATCHPAD
     */
    if (!onewire_reset()) {
        goto FAIL;
    }

    onewire_transmit(ONEWIRE_MEM_SKIP_ROM); /* only one device on bus */
    onewire_transmit(ONEWIRE_MEM_WRITE_SCRATCHPAD);
    onewire_transmit(addr);
    onewire_transmit(addr >> 8);
    onewire_transmit(b);

    /*
     * READ SCRATCHPAD
     */
    if (!onewire_reset()) {
        goto FAIL;
    }

    onewire_transmit(ONEWIRE_MEM_SKIP_ROM);
    onewire_transmit(ONEWIRE_MEM_READ_SCRATCHPAD);
    ta1 = onewire_receive();
    ta2 = onewire_receive();
    es = onewire_receive();

    /*
     * WRITE SCRATCHPAD TO MEMORY
     */
    if (!onewire_reset()) {
        goto FAIL;
    }

    onewire_transmit(ONEWIRE_MEM_SKIP_ROM);
    onewire_transmit(ONEWIRE_MEM_COPY_SCRATCHPAD);
    onewire_transmit(ta1);
    onewire_transmit(ta2);
    onewire_transmit(es);

    onewire_stop();

    /* copy takes 5 ms maximum */
    timer_sleep(5);

    return;

FAIL:
    onewire_stop();
}