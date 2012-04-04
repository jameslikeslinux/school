/*
 * queue.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120319
 * Lab:    3
 */

#include <zneo.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"

/* 64 bytes ought to be enough for anyone */
#define UART_BUFFER_SIZE 64

/* define bitmasks for extracting bits from registers */
#define UART_STATUS_RDA (1 << 7)
#define UART_STATUS_TDRE (1 << 2)
#define UART_CTL_TEN (1 << 7)
#define UART_CTL_REN (1 << 6)
#define UART_CTL_PEN (1 << 4)
#define UART_CTL_PODD (1 << 3)

/* globally accessible uarts */
uart_t uart0, uart1;

static void _uart_rx(uart_t *this) {
    /* if receive data is available, add it to the receive buffer */
    if (*(this->status) & UART_STATUS_RDA) {
        queue_enqueue(&this->rxbuffer, (void *) *(this->rx));
    }
}

/* interrupt handlers */
static void interrupt _uart0_rx_isr() {
    _uart_rx(&uart0);
}

static void interrupt _uart1_rx_isr() {
    _uart_rx(&uart1);
}

int uart_init(uart_t *this, int uartnum, int hz) {
    /* set interrupt handler for each uart */
    SET_VECTOR(UART0_RX, _uart0_rx_isr);
    SET_VECTOR(UART1_RX, _uart0_rx_isr);

    /* set all of the appropriate registers in the uart data structure */
    switch (uartnum) {
        case 0:
            PADD |= (1 << 4);   /* set PA4 as input */
            PAAFL |= (1 << 4);  /* set PA4 as RXD0 */
            PAAFL |= (1 << 5);  /* set PA5 as TXD0 */
            this->rx = &U0RXD;
            this->tx = &U0TXD;
            this->status = &U0STAT0;
            this->ctl = &U0CTL0;
            this->brd = &U0BR;
            this->irqh = &IRQ0ENH;
            this->irql = &IRQ0ENL;
            this->rxintbit = 4;
            break;

 
        case 1:
            PDDD |= (1 << 4);   /* set PD4 as input */
            PDAFL |= (1 << 4);  /* set PD4 as RXD0 */
            PDAFL |= (1 << 5);  /* set PD5 as TXD0 */
            this->rx = &U1RXD;
            this->tx = &U1TXD;
            this->status = &U1STAT0;
            this->ctl = &U1CTL0;
            this->brd = &U1BR;
            this->irqh = &IRQ2ENH;
            this->irql = &IRQ2ENL;
            this->rxintbit = 6;
            break;

        default:
            return 0;
    }

    this->hz = hz;
    uart_set_baud(this, 9600);
    uart_set_parity(this, NONE);

    queue_init(&this->rxbuffer, UART_BUFFER_SIZE);

    return 1;
}

int uart_get_baud(uart_t *this) {
    return this->baud;
}

void uart_set_baud(uart_t *this, int baud) {
    /* see "LIN-UART Baud Rate" section of product specifications for this calculation */
    *(this->brd) = this->hz / (16 * baud);
    this->baud = this->hz / (16 * *(this->brd));
}

uart_parity_t uart_get_parity(uart_t *this) {
    return this->parity;
}

void uart_set_parity(uart_t *this, uart_parity_t parity) {
    switch (parity) {
        case NONE:
            *(this->ctl) &= ~UART_CTL_PEN;
            break;

        case ODD:
            *(this->ctl) |= UART_CTL_PODD;
            *(this->ctl) |= UART_CTL_PEN;
            break;

        case EVEN:
            *(this->ctl) &= ~UART_CTL_PODD;
            *(this->ctl) |= UART_CTL_PEN;
            break;

        default:
            return;
    }

    this->parity = parity;
}

void uart_start(uart_t *this) {
    /* enable receive interrupts */
    EI();
    *(this->irqh) |= (1 << this->rxintbit);
    *(this->irql) |= (1 << this->rxintbit);

    /* enable transmitter */
    *(this->ctl) |= UART_CTL_TEN;

    /* enable receiver */
    *(this->ctl) |= UART_CTL_REN;
}

int uart_kbhit(uart_t *this) {
    unsigned short istat;
    int hit;

    /* disable interrupts */
    /*
     * (I disable interrupts whenever messing with the buffers
     *  because I do not want an interrupt to clobber it)
     */
    istat = TDI();

    if (queue_isempty(&this->rxbuffer)) {
        /* I might have missed an interrupt */
        _uart_rx(this);
    }

    hit = !queue_isempty(&this->rxbuffer);

    /* restore interrupts */
    RI(istat);

    return hit;
}

char uart_getch(uart_t *this) {
    unsigned short istat;
    void *data;

    /* disable interrupts */
    istat = TDI();

    while (!queue_dequeue(&this->rxbuffer, &data)) {
        /* while there's nothing in the queue, poll for data */
        _uart_rx(this);
    }

    /* restore interrupts */
    RI(istat);

    return (char) data;
}

void _uart_putch(uart_t *this, char c) {
    /*
     * There is no need to buffer transmit characters...
     * We buffer received characters because we have no control
     * over when data comes in, but we do have control of when
     * data goes out.  I see no need to complicate transmission
     * with buffers and interrupts.
     */

    /* wait until the transmit register is empty */
    while (!(*(this->status) & UART_STATUS_TDRE));

    /* write the byte to the transmit register */
    *(this->tx) = c;
}

void uart_putch(uart_t *this, char c) {
    if (c == '\n') {
        /* for some reason, \n alone doesn't quite do what I expect */
        _uart_putch(this, '\r');
    }

    _uart_putch(this, c);
}

/*
 * XXX: this is really lazy...
 * In the interest of saving time this just calls vsprintf
 */
void myprintf(char *fmt, ...) {
    va_list fmtargs;
    char buffer[256];
    int i;

    /*
     * unfortunately, vsnprintf is not available so this WILL
     * overflow buffer if the resulting string is longer than 255
     * characters!
     */
    va_start(fmtargs, fmt);
    vsprintf(buffer, fmt, fmtargs);
    va_end(fmtargs);

    /* write resulting string to uart0 */
    for (i = 0; i < strlen(buffer); i++) {
        uart_putch(&uart0, buffer[i]);
    }
}