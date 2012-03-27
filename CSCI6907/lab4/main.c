/*
 * main.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120326
 * Lab:    4
 *
 * Description: This lab implements the PS/2 protocol for keyboards to
 * be able to read scancodes from a keyboard, display them to a serial
 * port and LED array, and translate the scancodes in to real keypresses.
 * 
 * I changed the way to stop the 'scan' and 'type' commands from one
 * one button press as in the lab description, to a double-press, so as
 * to not conflict with the 'switch' command.  I also modified my speaker
 * code from Lab 2 to work across PC0 and PC1, as opposed to just PC1 and
 * GND.
 *
 * Other files: button.{c,h} cli.{c,h} led.{c,h} ledarray.{c,h} matrix.h
 * queue.{c,h} timer.{c,h} uart.{c,h} utils.{c,h}
 *
 * Problems: Ensuring that I captured a whole scan code without being
 * interrupted turned out to be a real challenge.  This site:
 *
 *   http://www.computer-engineering.org/ps2protocol/
 *
 * was a very useful resource.  After reading it, I reconfigured my
 * inputs to be in open-collector mode, and inhibited the keyboard from
 * using the bus by dropping the clock line low.  Then I could control
 * when I wanted to read from the keyboard--after disabling interrupts.
 *
 * Comments: Good lab that introduced me to the issues with really low-level
 * timing stuff.  The speaker turned out to be a sort of canary in the coal
 * mine.  I thought things were working until I enabled the speaker and
 * noticed it hanging (playing a tone until I pressed another key).  That
 * led me to see the interrupt problem.
 *
 * Enhancements: I display scan codes on the LED display as they're being
 * typed.
 *
 * Questions:
 *
 * 1. What technique did you use for watching the data and clock lines?
 * I configured my data and clock lines for open-collector mode so that
 * I could, in a sense, tell the keyboard when I was ready to read data.
 * I quickly discovered that timing was critical.  Once the keyboard
 * dropped the clock line low, I could not be interrupted or I would miss
 * the frame.  So I had to disable interrupts, then raise the clock line,
 * poll for activity on the clock line, and clock in data bits whenever
 * the clock was low.  In my "get_scancode" function, I alternate between
 * polling for keypresses, and having interrupts enabled, so that all of 
 * the interrupt-driven functions continue to work while I'm waiting for
 * keypresses (leds, buttons, timers, etc).
 *
 * 2. Did you use the oscilloscopes in the class room to help debug your
 * lab setup?
 *
 * I did not use the oscilloscope.  Using the information from the web site
 * listed above, I was able to work with the protocol just fine "blind," as
 * it were.  Once I was able to reliably read scancodes, I used this table
 * to help me convert them into keypresses and characters:
 *
 *   http://www.computer-engineering.org/ps2keyboard/scancodes2.html
 *
 * 3. What difficulties did you have keeping track of when to sample the
 * data line?
 *
 * As I said above, I had all sorts of trouble until I disabled interrupts
 * from the moment I started polling for data until after I read the stop
 * bit.  In order to ensure the keyboard would not send me codes while
 * interrupts were enabled, I had to inhibit the keyboard by dropping the
 * clock line low.  That instructs the keyboard to buffer keypresses until
 * the line is raised for at least 50 us.
 *
 * 4. What did you try that didn't work?
 *
 * I tried disabling interrupts immediately after I saw the keyboard drop
 * the clock line low, and that helped except for about one in every fifty
 * keypresses, when an interrupt would just happen to occur between the time
 * the keyboard started to send a frame and the time I disabled interrupts.
 * The real breakthrough was learning to inhibit the keyboard from talking
 * on the bus.
 *
 * 5. Describe issues with integrating LED display library...
 *
 * Because the 'get_scancode' function alternates between polling for
 * scancodes with interrupts disabled, and enabling interrupts, some timer
 * interrupts do get missed.  This manifests itself as a slightly reduced
 * LED scan rate (which introduces some minor flickering).  With the timing
 * required for inhibiting keyboard communication (~100 us), I do not see a
 * way around that.
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
#include "keyboard.h"
#include "speaker.h"

#define NUM_BUTTONS 3
static button_t buttons[NUM_BUTTONS];
static char *button_text[NUM_BUTTONS];
static keyboard_t keyboard;
static volatile int kbdstop;

void _stop(void *data) {
    kbdstop = 1;
    keyboard_stop(&keyboard);
}

static rom char *scan_help = "scan\n - will display scancodes from the PS/2 keyboard";
int _scan(int argc, char *argv[]) {
    char c;

    kbdstop = 0;

    while (1) {
        c = keyboard_get_scancode(&keyboard);

        if (kbdstop) {
            myprintf("\n");
            return CMD_SUCCESS;
        }

        myprintf("%02X ", c);
    }

    /* this shouldn't happen */
    return CMD_ERROR;
}

static rom char *type_help = "type\n - will display characters from the PS/2 keyboard";
int _type(int argc, char *argv[]) {
    char *str;
    int written;

    kbdstop = 0;
    written = 0;

    while (1) {
        str = keyboard_get_string(&keyboard);

        if (kbdstop) {
            myprintf("\n");
            return CMD_SUCCESS;
        }

        if (!strcmp(str, "\x1b")) {
            /* handle escape */
            _stop(NULL);
            myprintf("\n");
            return CMD_SUCCESS;
        } else if (!strcmp(str, "\x7f")) {
            /* handle backspace */
            if (written > 0) {
                myprintf("\b \b");
                written--;
            }
        } else {
            myprintf("%s", str);
            written += strlen(str);
        }
    }

    /* this shouldn't happen */
    return CMD_ERROR;
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
    int i;

     _set_oscillator();

    uart_init(&uart0, 0, 18432000);
    uart_set_baud(&uart0, 57600);
    uart_start(&uart0);
    clear_screen();

    timer_init(18432000);

    speaker_init(18432000);
    
    ledarray_init();
    ledarray_set_scroll_rate(40);

    /* initialize the three physical buttons */
    button_init(&buttons[0], &PDDD, &PDIN, (1 << 3));
    button_init(&buttons[1], &PFDD, &PFIN, (1 << 6));
    button_init(&buttons[2], &PFDD, &PFIN, (1 << 7));
    button_text[0] = NULL;
    button_text[1] = NULL;
    button_text[2] = NULL;

    for (i = 0; i < NUM_BUTTONS; i++) {
        /* using double press so I don't have to conflict with the switch command */
        button_on_double_press(&buttons[i], _stop, NULL);
    }

    keyboard_init(&keyboard, &PDIN, &PDOUT, &PDDD, &PDOC, 0, 1);

    /* this is where the magic happens */
    cli_init();
    cli_add_command("scan", scan_help, _scan);
    cli_add_command("type", type_help, _type);
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
