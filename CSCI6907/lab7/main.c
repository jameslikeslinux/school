/*
 * main.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120403
 * Lab:    7
 *
 * Description: This lab implements single wire EEPROM chips.  It uses the
 * same basic framework and commands as Lab 5, but adds low-level bit-banging
 * libraries to communicate over 1-wire bus and UNI/O bus.
 *
 * Other files: button.{c,h} cli.{c,h} i2c.{c,h} i2c_mem.{c,h} led.{c,h}
 * ledarray.{c,h} matrix.h onewire.{c,h} onewire_mem.{c,h} queue.{c,h}
 * spi.{c,h} spi_mem.{c,h} timer.{c,h} uart.{c,h} unio.{c,h} unio_mem.{c,h}
 * utils.{c,h}
 *
 * Problems: Lab is working completely...See questions section for details.
 *
 * Enhancements: I implemented both the 1-wire DS2433 chip and the UNI/O
 * 11AA160 chip.
 *
 * Questions:
 *
 * 1. What device did you implement a bit-bang library for?  Why did you
 * select that one?
 *
 * At first I chose to implement the the 11AA160 chip because I already had
 * experience implementing other Microchip EEPROMs in Lab 5 and the commands
 * and operation of the UNI/O chip are similar to the SPI and I2C chips.  Also,
 * the datasheet is easier to read than the Dallas one.  And also, UNI/O
 * seemed like a simpler protocol, with uniform timing (the bitperiod) and
 * Manchester encoding.  At first, 1-wire seemed more fragile and likely to
 * cause trouble if I didn't get the timing right.  I also didn't want to
 * worry about whether the parisitic power of the DS2433 was working properly.
 *
 * However, after implementing the UNI/O protocol and some very basic
 * commands and having no luck, I decided to try to implement the 1-wire
 * protocol and the DS2433.  It didn't work either.  I was convinced my timing
 * wasn't accurate enough.
 *
 * 2. Schematic:
 *
 *         DS2433          ^ Vcc                      11AA160      ^ Vcc
 *                        |                                       |
 *       1   2   3        /                        Vss SCIO Vcc   |
 *       .   .   .        \ 1K                      .   .   .     |
 *       |   |   NC       /                         |   |   |     |
 *       |   |            |                         _   |   -------
 *       |   |            |                         -   |
 *       |   ------------------------ PA0               |
 *       _                            PA1 ---------------
 *       -
 *
 * 3. Did you have any trouble finding datasheets or any necessary info?
 * 
 * No.
 *
 * 4. Did you need any pull up resistors?  Why or why not?
 *
 * Yes.  Both 1-wire and UNI/O are open-collector busses.  In the case of
 * 1-wire the bus is required to idle high to power the parasitic device.
 * For UNI/O, a pull-up is just a recommendation, not a requirement.
 * 
 * At first, I used the ZNEO's internal pull-ups, as I did for Lab 5.
 * Then, when I attached the oscilloscope to the 1-wire bus, I noticed
 * the bus was slow to rise from low to high (taking about 5 micros).
 * I figured this was due to the parasitic capacitor trying to draw power
 * from the ZNEO's low pulled-up current.  I disabled the internal pull-up
 * on the 1-wire bus and replaced it with a 1K ohm resistor attached to Vcc.
 * That fixed the problem.  (The specification recommends 2.2K ohm resistor
 * but I don't have one...a 1K ohm doesn't hurt, it just wastes more power
 * during idle).
 *
 * 5. How did you verify your delays?
 *
 * I had no choice but to use an oscilloscope.  When working down at the
 * microsecond level, a few extra operations can throw your timing completely
 * off.  I wrote a simple loop that toggled a pin on and off, delaying each
 * time.  Then I could see the signal on the oscilloscope and measure the
 * period.  It turned out my function had a 20 micros overhead!  The majority
 * of that time was spent doing the arithmetic to calculate the reload value.
 * Because I had to have delays of less than 20 micros (the smallest is 5 us),
 * I eliminated the arithmatic and require functions to pass the reload value
 * pre-calculated.  For the 18.432 MHz clock, the reload value is
 * (18.432 * micros).  That got my overhead down to 2.5 micros, which I
 * subtract away.  Now I have an accurate delay function from 3 to 3555 micros.
 *
 * The oscilloscope also helped me identify an issue with my UNI/O library.
 * Whenever I sent a NoMAK, I wouldn't read a SAK back.  I could see on the
 * oscilloscope that the device was sending a SAK, but only after I checked for
 * it.  It turns out I wasn't releasing the bus when I went to read, so of
 * course the device couldn't send the SAK.
 *
 * Once I got the delay function accurate, and the little UNI/O issue worked
 * out, the 1-wire and UNI/O libraries I wrote the night before just worked.
 *
 * 6. What was the hardest part of this lab?
 *
 * The hardest part of this lab was working from home without an oscilloscope.
 * This lab is just about impossible without one.  When I got to class and 
 * attached the scope lead to my UNI/O bus, it was a revelation.  I could see
 * the complete signal going back and forth.  I could read and count the
 * Manchester encoded bits and verify that my timing was accurate.
 */

#include <zneo.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "timer.h"
#include "ledarray.h"
#include "button.h"
#include "utils.h"
#include "cli.c"
#include "uart.h"
#include "spi.h"
#include "spi_mem.h"
#include "i2c.h"
#include "i2c_mem.h"
#include "unio.h"
#include "unio_mem.h"
#include "onewire.h"
#include "onewire_mem.h"

#define NUM_BUTTONS 3
static button_t buttons[NUM_BUTTONS];
static char *button_text[NUM_BUTTONS];

/* shared command function for 'i2c' and 'spi' commands */
int _do_memory_command(int argc, char *argv[], void (*write)(unsigned short addr, unsigned char b), unsigned char (*read)(unsigned short addr), unsigned short max_addr) {
    int addr, length, i, j, numbytes;
    unsigned char bytes[16], b;

    if (argc != 2 && argc != 3 && argc != 4) {
        return CMD_SHOW_HELP;
    }

    /* get length arg if we're not doing a write */
    length = INT_MAX;
    if (argc == 4 && strcmp(argv[1], "write") && (length = atoi(argv[3])) == 0) {
        myprintf("Invalid length.\n");
        return CMD_ERROR;
    }

    /* get address argument */
    addr = 0;
    if (argc >= 3 && ((addr = hex_string_to_integer(argv[2])) == -1 || addr > max_addr)) {
        myprintf("Invalid address.\n");
        return CMD_ERROR;
    }

    if (!strcmp(argv[1], "erase")) {
        /* erase -> write zeros to 'length' addresses, starting at 'addr' */
        for (i = 0; i < length && addr <= max_addr; i++, addr++) {
            if (i % 16 == 0) {
                /* print progress indicator */
                myprintf(".");
            }
            write(addr, 0);
        }
        myprintf("\n");
    } else if (!strcmp(argv[1], "fill")) {
        /* fill -> write incrementing values to 'length' addresses, starting at 'addr' */
        for (i = 0; i < length && addr <= max_addr; i++, addr++) {
            if (i % 16 == 0) {
                /* print progress indicator */
                myprintf(".");
            }
            write(addr, i);
        }
        myprintf("\n");
    } else if (!strcmp(argv[1], "dump")) {
        numbytes = 0;

        for (i = 0; i < length && addr <= max_addr; i++, addr++) {
            b = read(addr);
            if (i == 0) {
                /* start dump output */
                numbytes = 0;
                bytes[numbytes++] = b;
                myprintf("%07x %02x", addr, b);
            } else if (i % 16 == 0) {
                /* start new line of dump output */

                /* first, print ascii representation of preceding line */
                myprintf("  ");
                for (j = 0; j < numbytes; j++) {
                    if (bytes[j] >= 32 && bytes[j] <= 126) {
                        myprintf("%c", bytes[j]);
                    } else {
                        myprintf(".");
                    }
                }

                numbytes = 0;
                bytes[numbytes++] = b;
                myprintf("\n%07x %02x", addr, read(addr));
            } else if (i % 2 == 1) {
                /* end a word of output */
                bytes[numbytes++] = b;
                myprintf("%02x ", read(addr));
            } else {
                /* continue a word of output */
                bytes[numbytes++] = b;
                myprintf("%02x", read(addr));
            }
        }

        /* fill in spaces if last line is not complete */
        for (; i % 16 != 0; i++) {
            myprintf("  ");
            if (i % 2 == 1) {
                myprintf(" ");
            }
        }

        if (numbytes > 0) {
            /* print ascii representation of preceding line */
            myprintf("  ");
            for (j = 0; j < numbytes; j++) {
                if (bytes[j] >= 32 && bytes[j] <= 126) {
                    myprintf("%c", bytes[j]);
                } else {
                    myprintf(".");
                }
            }
        }

        myprintf("\n");
    } else if (!strcmp(argv[1], "write") && argc == 4) {
        /* write -> write bytes from the fourth arg to 'addr' */
        for (i = 0; i < strlen(argv[3]) && addr <= max_addr; i++, addr++) {
            write(addr, argv[3][i]);
        }
    } else {
        return CMD_SHOW_HELP;
    }

    return CMD_SUCCESS;
}

static rom char *i2c_help = "i2c <erase|fill|dump> [start [length]]\ni2c write <address> <\"text\">";
int _i2c(int argc, char *argv[]) {
    return _do_memory_command(argc, argv, i2c_mem_write, i2c_mem_read, I2C_MEM_MAX_ADDR);
}

static rom char *spi_help = "spi <erase|fill|dump> [start [length]]\nspi write <address> <\"text\">";
int _spi(int argc, char *argv[]) {
    return _do_memory_command(argc, argv, spi_mem_write, spi_mem_read, SPI_MEM_MAX_ADDR);
}

static rom char *unio_help = "unio <erase|fill|dump> [start [length]]\nunio write <address> <\"text\">";
int _unio(int argc, char *argv[]) {
    return _do_memory_command(argc, argv, unio_mem_write, unio_mem_read, UNIO_MEM_MAX_ADDR);
}

static rom char *onewire_help = "onewire <erase|fill|dump> [start [length]]\nonewire write <address> <\"text\">";
int _onewire(int argc, char *argv[]) {
    return _do_memory_command(argc, argv, onewire_mem_write, onewire_mem_read, ONEWIRE_MEM_MAX_ADDR);
}

static rom char *echo_help = "echo [\"text\"]\n - echos the \"text\" without the quotes to the serial port";
int _echo(int argc, char *argv[]) {
    if (argc > 2) {
        return CMD_SHOW_HELP;
    }

    if (argc == 1) {
        myprintf("\n");
    } else if (argc == 2) {
        myprintf("%s\n", argv[1]);
    }

    return CMD_SUCCESS;
}

static rom char *display_help = "display [\"text\"]\n - displays \"text\" scolling across the LED display";
int _display(int argc, char *argv[]) {
    if (argc > 2) {
        return CMD_SHOW_HELP;
    }

    if (argc == 1) {
        ledarray_setstring("");
    } else if (argc == 2) {
        ledarray_setstring(argv[1]);
    }

    return CMD_SUCCESS;
}

static rom char *set_help = "set prompt <\"text\">\n - changes prompt text (15 character limit)";
int _set(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "prompt")) {
        return CMD_SHOW_HELP;
    }

    cli_set_prompt(argv[2]);

    return CMD_SUCCESS;
}

static rom char *hex_help = "hex [decimal number]\n - displays the number in hexadecimal on the LED";
int _hex(int argc, char *argv[]) {
    int decimal;

    if (argc > 2) {
        return CMD_SHOW_HELP;
    }

    if (argc == 1) {
        ledarray_setstring("");
    } else if (argc == 2) {
        /* check that 'decimal number' can actually be parsed into a number */
        decimal = atoi(argv[1]);

        if (decimal == 0 && argv[1][0] != '0') {
            return CMD_SHOW_HELP;
        }

        ledarray_sethexadecimal(decimal);
    }

    return CMD_SUCCESS;
}

void button_pressed(void *data) {
    /* called if a button is single-pressed */
    if (data) {
        cli_inject_text((char *) data);
    }
}

static rom char *switch_help = "switch <1|2|3> [\"text\"]\n - configures button \'n\' to run \'text\' as if it were a command";
int _switch(int argc, char *argv[]) {
    int button;
    char *old_text;

    if (argc != 2 && argc != 3) {
        return CMD_SHOW_HELP;
    }

    button = atoi(argv[1]);

    if (button < 1 || button > NUM_BUTTONS) {
        myprintf("Invalid button.\n");
        return CMD_SHOW_HELP;
    }

    /* keep pointer to current text because it needs to be freed later */
    old_text = button_text[button - 1];

    if (argc == 3) {
        /*
         * we have to malloc new space because the string has to persist
         * long after this function has been called (and technically, the string can
         * be of arbitrary length)
         */ 
        button_text[button - 1] = (char *) malloc((strlen(argv[2]) + 1) * sizeof(char));
        if (button_text[button - 1]) {  /* check memory allocation sucess */
            strcpy(button_text[button - 1], argv[2]);
            button_text[button - 1][strlen(argv[2])] = '\0';
        }
    } else {
        button_text[button - 1] = NULL;
    }

    /* register callback */
    button_on_single_press(&buttons[button - 1], button_pressed, button_text[button - 1]);

    if (old_text) {
        /*
         * we wait to free the old text because it is possible for
         * a button to be pressed while it is being updated (race condition)
         */
        free(old_text);
    }
    
    return CMD_SUCCESS;
}

static rom char *port_help = "port <A-H>\n - displays how the IO port <A-H> is configured";
static rom char *port_header = "BIT  I/O   IN  OUT  ALT FUNC  HIGH DRIVE  OPEN DRAIN  PULL UP\n";
int _port(int argc, char *argv[]) {
    char port;
    unsigned char volatile near *dd, *in, *out, *afh, *afl, *hde, *oc, *pue;
    int i;

    if (argc != 2) {
        return CMD_SHOW_HELP;
    }

/* a macro to help set registers shared by all ports */
#define set_port(x) dd  = &P ## x ## DD;  \
                    in  = &P ## x ## IN;  \
                    out = &P ## x ## OUT; \
                    hde = &P ## x ## HDE; \
                    oc  = &P ## x ## OC;  \
                    pue = &P ## x ## PUE;
    
    port = toupper(argv[1][0]);
    afh = NULL;
    afl = NULL;

    switch (port) {
        case 'A':
            set_port(A);
            afh = &PAAFH;   /* not all ports have all the registers, */
            afl = &PAAFL;   /* so they can't be defined in the macro above */
            break;

        case 'B':
            set_port(B);
            afl = &PBAFL;
            break;

        case 'C':
            set_port(C);
            afh = &PCAFH;
            afl = &PCAFL;
            break;

        case 'D':
            set_port(D);
            afl = &PDAFL;
            break;

        case 'E':
            set_port(E);
            break;

        case 'F':
            set_port(F);
            afl = &PFAFL;
            break;

        case 'G':
            set_port(G);
            afl = &PGAFL;
            break;

        case 'H':
            set_port(H);
            afh = &PHAFH;
            afl = &PHAFL;
            break;

        default:
            myprintf("Invalid port.\n");
            return CMD_SHOW_HELP;
    }

    myprintf(port_header);
    for (i = 0; i < 8; i++) {
        myprintf("%-3d  %3c  %3d  %3d  %8d  %10s  %10s  %7s\n",
            i,
            ((*dd >> i) & 1) ? 'I' : 'O',
            (*in >> i) & 1,
            (*out >> i) & 1,
            (((*afh >> i) & 1) << 1) | ((*afl >> i) & 1),
            ((*hde >> i) & 1) ? "Yes" : "No",
            ((*oc >> i) & 1) ? "Yes" : "No",
            ((*pue >> i) & 1) ? "Yes" : "No"
        );
    }

    return CMD_SUCCESS;
}

static rom char *uart0_help = "uart0 [speed [baudrate]] [parity [even|odd|none]]\n - displays and sets uart0 settings immediately";
int _uart0(int argc, char *argv[]) {
    int baud;
    char *parity;
    uart_parity_t set_parity;

    switch (uart_get_parity(&uart0)) {
        case NONE:
            parity = "none";
            break;
        case ODD:
            parity = "odd";
            break;
        case EVEN:
            parity = "even";
            break;
        default:
            return CMD_ERROR;
    }

    /*
     * Functions like these are why 'getopt' was invented.
     */

    if (argc == 1) {
        myprintf("Speed: %d\n", uart_get_baud(&uart0));
        myprintf("Parity: %s\n", parity);
    } else if (argc == 2) {
        if (!strcmp(argv[1], "speed")) {
            myprintf("Speed: %d\n", uart_get_baud(&uart0));
        } else if (!strcmp(argv[1], "parity")) {
            myprintf("Parity: %s\n", parity);
        } else {
            return CMD_SHOW_HELP;
        }
    } else if (argc == 3) {
        if (!strcmp(argv[1], "speed")) {
            baud = atoi(argv[2]);
            if (baud > 0) {
                uart_set_baud(&uart0, baud);
            } else {
                myprintf("Invalid baud rate.\n");
                return CMD_ERROR;
            }
        } else if (!strcmp(argv[1], "parity")) {
            if (!strcmp(argv[2], "even")) {
                uart_set_parity(&uart0, EVEN);
            } else if (!strcmp(argv[2], "odd")) {
                uart_set_parity(&uart0, ODD);
            } else if (!strcmp(argv[2], "none")) {
                uart_set_parity(&uart0, NONE);
            } else {
                myprintf("Invalid parity\n");
                return CMD_SHOW_HELP;
            }
        } else {
            return CMD_SHOW_HELP;
        }        
    } else if (argc == 5) {
        if (!strcmp(argv[1], "speed") && !strcmp(argv[3], "parity")) {
            baud = atoi(argv[2]);
            if (baud <= 0) {
                myprintf("Invalid baud rate.\n");
                return CMD_ERROR;
            }

            if (!strcmp(argv[4], "even")) {
                set_parity = EVEN;
            } else if (!strcmp(argv[4], "odd")) {
                set_parity = ODD;
            } else if (!strcmp(argv[4], "none")) {
                set_parity = NONE;
            } else {
                myprintf("Invalid parity\n");
                return CMD_SHOW_HELP;
            }

            uart_set_baud(&uart0, baud);
            uart_set_parity(&uart0, set_parity);
        } else {
            return CMD_SHOW_HELP;
        }
    } else {
        return CMD_SHOW_HELP;
    }

    return CMD_SUCCESS;
}

static rom char *info_help = "info\n - displays part number, serial port baud rate, and compile timestamp";
int _info(int argc, char *argv[]) {
    char part_number[21];

    /* enable information area access */
    FCTL |= (1 << 7);

    /* copy part number into ram (20 bytes as per specs) */
    memcpy(part_number, (void *) 0x40, 20);
    part_number[20] = '\0';

    myprintf("Part Number: %s\n", part_number);
    myprintf("Serial Baud Rate: %d\n", uart_get_baud(&uart0));
    myprintf("Compiled Timestamp: %s %s\n", __DATE__, __TIME__);

    return CMD_SUCCESS;
}

static rom char *help_help = "?\n - display a menu of CLI commands";
int _help(int argc, char *argv[]) {
    cli_show_help();
    return CMD_SUCCESS;
}

void _set_oscillator() {
    /*
     * SAMPLE CODE FROM INSTRUCTOR
     *
     * Good enough for selecting the external clock
     * TODO: Figure out how to use SYS_CLK_SRC, SYS_CLK_FREQ
     */

    // unlock the oscillator control register
    OSCCTL = 0xE7; OSCCTL = 0x18;  
   
    // follow with ONE of these TWO settings

    // A0 = 1010_0000 = internal 5.52 MHz
    //OSCCTL = 0xA0;                    

    // 61 = 0110_0001 = external 18.432 Hz
    OSCCTL = 0x61;
}


void main() {
    int i;

    _set_oscillator();

    uart_init(&uart0, 0, 18432000);
    uart_set_baud(&uart0, 57600);
    uart_start(&uart0);
    clear_screen();

    timer_init(18432000);
    
    ledarray_init();
    ledarray_set_scroll_rate(40);

    /* initialize the three physical buttons */
    button_init(&buttons[0], &PDDD, &PDIN, (1 << 3));
    button_init(&buttons[1], &PFDD, &PFIN, (1 << 6));
    button_init(&buttons[2], &PFDD, &PFIN, (1 << 7));
    button_text[0] = NULL;
    button_text[1] = NULL;
    button_text[2] = NULL;

    /* initialize busses and memory devices */
    spi_init(18432000);
    spi_mem_init();

    i2c_init(18432000);
    i2c_mem_init();

    unio_init();
    unio_mem_init();

    onewire_init();
    onewire_mem_init();

/* to test delay function on oscilloscope */
#if 0
    while (1) {
        if (PAIN & 1) {
            PAOUT &= ~1;
        } else {
            PAOUT |= 1;
        }

        delay_micros(/* 18.432 * 50 us = */ 922);
    }
#endif

    /* this is where the magic happens */
    cli_init();
    cli_add_command("i2c", i2c_help, _i2c);
    cli_add_command("spi", spi_help, _spi);
    cli_add_command("unio", unio_help, _unio);
    cli_add_command("onewire", onewire_help, _onewire);
    cli_add_command("echo", echo_help, _echo);
    cli_add_command("display", display_help, _display);
    cli_add_command("set", set_help, _set);
    cli_add_command("hex", hex_help, _hex);
    cli_add_command("switch", switch_help, _switch);
    cli_add_command("port", port_help, _port);
    cli_add_command("uart0", uart0_help, _uart0);
    cli_add_command("info", info_help, _info);
    cli_add_command("?", help_help, _help);
    cli_start();
}
