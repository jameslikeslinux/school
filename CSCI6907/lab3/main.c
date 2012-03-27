/*
 * main.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120319
 * Lab:    3
 *
 * Description: In this lab, I wrote code to configure, and send and
 * receive data from the on-board UARTs, and used those functions to
 * develop a serial command line interface.  I took care to implement
 * simple screen commands like CTRL-C and CTRL-L (try them) and
 * command string tokenization that understands quoted strings and
 * escaped quotes.  That way, command arguments can be passed as tokens
 * directly to command functions similar to how Unix passes arguments
 * to 'main' functions.  All of the commands required by the lab have
 * been implemented (except 'timer' which I ran out of time to do, and
 * is similar enough in concept to the 'port' command that I felt it
 * could be dropped).
 *
 * Other files: button.{c,h} cli.{c,h} led.{c,h} ledarray.{c,h} matrix.h
 * queue.{c,h} timer.{c,h} uart.{c,h} utils.{c,h}
 *
 * Problems: I found that not all of the ports expose all of the same
 * registers which made implementing a generic 'port' command very
 * difficult.  I worked around this by reading from NULL for register
 * that ports do not have.
 *
 * Also, it wasn't completely clear from the lab description that we had
 * to implement the UART ourselves (as opposed to using sio.h).  I had to
 * refactor my CLI half-way through the lab to use my own putch and getch 
 * as opposed to the Zilog provided ones.  This took time away from
 * implementing all of the commands as fully as I would have liked, however
 * I think I demonstrated the concepts well enough.
 *
 * Finally, using a transmit buffer and interrupt in the UART was nothing
 * but trouble (as is also the case in my project which uses a MAX3110
 * UART).  I am of the opinion that buffers and interrupts shouldn't be
 * necessary when transmitting data because we can control when we send
 * data.  Buffers and interrupts make sense for receive because we can't
 * control when we receive data.  Using a simple polling strategy when
 * transmitting is much more robust.
 *
 * Questions:
 *
 * 1. What problems did you have using your LED library?  Did you have to
 * make any changes so it would work with this lab?
 *
 * I did not have any problems with the LED library.  The only change I
 * made was probably not necessary, but I reduced the priority of the 
 * timer interrupt so UART interrupts take priority if they should ever
 * fire at the same time.
 *
 * 2. What problems did using interrupts on transmit and receive cause
 * for you?
 *
 * Interrupts on receive were no problem.  They worked correctly the first
 * time and every time.  Interrupts for transmit were problematic.  For
 * typing speed it would work pretty well, but for faster data transmission
 * (such as a large printf statement), it would get hung up.  My guess would
 * be that the UART would send an transmit empty interrupt while it was still
 * processing the last transmit register empty interrupt, causing data to not
 * get flushed from the buffer.  I already explained why I don't think
 * transmit buffers/interrupts are beneficial in any case.
 *
 * 3. How big is your input and output buffer?  How did you determine the
 * size?  What happens when you exceed the buffer size?
 *
 * The input buffer is 64 bytes.  I do not use an output buffer (it is
 * unnecessary in my opinion).  I chose 64 bytes because that is also the
 * size used by UART implementation for the Arduino and it seemed like 
 * if it was good enough for them, it should be good enough for me.  A
 * more scientific approach would have me looking at how fast it would be
 * possible to fill the buffer at a specified baud rate vs how often I
 * plan to flush the buffer in my code.
 *
 * If the buffer size is exceeded, received data starts to get dropped,
 * as that is the only thing I can do considering I did not implement
 * flow control.
 *
 * 4. How did you deal with pressing the text from a button press?
 *
 * I implemented a "command queue" that commands get inserted into in
 * order to be executed.  That way, the source of the commands doesn't
 * matter.  It could be text from the UART, or text from a function call
 * as triggered by a button press.  Also, it ensures commands are executed
 * by the main loop rather than from within an interrupt (keeps interrupts
 * short).  The command queue method also lets buttons be pressed while
 * another (potentially long running) command is running.  The commands
 * get queued up and execute one after the other.
 *
 * 5. Describe (briefly) how you will add new commands to the CLI.
 *
 * Simple:  Implement a 'int xxx(int argc, char *argv[])' function
 * and pass it to 'cli_add_command'.
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

#define NUM_BUTTONS 3
static button_t buttons[NUM_BUTTONS];
static char *button_text[NUM_BUTTONS];

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

    myprintf("BIT  I/O   IN  OUT  ALT FUNC  HIGH DRIVE  OPEN DRAIN  PULL UP\n");
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
    char c;

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

    /* this is where the magic happens */
    cli_init();
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
