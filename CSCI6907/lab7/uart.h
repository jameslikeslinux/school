/*
 * uart.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120319
 * Lab:    3
 */

#ifndef UART_H
#define UART_H

#include <zneo.h>
#include "queue.h"

typedef enum {
    NONE,
    ODD,
    EVEN
} uart_parity_t;

typedef struct {
    /*
     * PRIVATE
     */
    queue_t rxbuffer;
    int hz, baud;
    uart_parity_t parity;
    int rxintbit;
    unsigned volatile char near *rx, *tx, *status, *ctl, *irqh, *irql;
    unsigned volatile short near *brd;
} uart_t;

/*
 * Initialize a uart object
 *
 * this - must be '&uart0' or '&uart1'
 * uartnum - must be 0 or 1
 * hz - the system clock rate
 * returns - 1 if initialized successfully, 0 otherwise
 */
int uart_init(uart_t *this, int uartnum, int hz);

/*
 * Data cannot be sent or received until this function is called
 */
void uart_start(uart_t *this);

int uart_get_baud(uart_t *this);
void uart_set_baud(uart_t *this, int baud);
uart_parity_t uart_get_parity(uart_t *this);
void uart_set_parity(uart_t *this, uart_parity_t parity);

/*
 * returns - 1 if data is available for reading, 0 otherwise
 */
int uart_kbhit(uart_t *this);

/*
 * returns - the next byte available (blocking if necessary)
 */
char uart_getch(uart_t *this);

/*
 * Blocks until 'c' can be written to the specified uart
 */
void uart_putch(uart_t *this, char c);

/*
 * Will format print output for uart0...
 *
 * Just like 'printf' except your board will probably blow up
 * if the resulting string is over 255 characters
 * (see the implementation for why)
 */
void myprintf(char *fmt, ...);

/*
 * THE ONLY VALID UART OBJEJCTS!
 */
extern uart_t uart0, uart1;

#endif