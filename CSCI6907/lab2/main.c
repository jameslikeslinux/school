/*
 * main.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120228
 * Lab:    2
 *
 * Description: This lab uses a timer to generate accurate tones from a speaker
 * connected across T1OUT (PC1) and GND.  It also implements an RTTTL parser
 * along with three RTTTL tunes that can be parsed and played through the
 * speaker at the press of one of the three buttons.
 *
 * Other files: button.{c,h} led.{c,h} ledarray.{c,h} matrix.h timer.{c,h}
 *              rtttl.{c,h} rtttl_note.{c,h} utils.{c,h}
 *
 * Problems: In order to get sound to play from the speaker, I had to connect
 * one speaker pin to the T1OUT pin, and the other to GND, not PC0/T1IN as
 * specified in the lab.  It seems to me that that shouldn't work/be necessary
 * in any case.  PC0/T1IN is a high-impedance digital logic input so you
 * wouldn't get the current flow necessary for sound, plus there is no
 * functional advantage to having the speaker sinking to a timer input.  It's a
 * waste of a GPIO pin as well.  GND is the more suitable sink for the speaker.
 *
 * Enhancements: I added a basic user interface that prompts the user to press
 * a button, displays the name of the song being played, and loops the song
 * until a button is double-pressed.
 *
 * Questions:
 *
 * 1. Could you have used other IO pins.  Why/why not?
 *
 * As I wrote, I did have to sink the speaker to GND rather than a GPIO pin.
 * For the speaker source, I could have connected it to another timer output,
 * such as PC7/T2OUT, or PA1/T0OUT (if T0 wasn't being used for event control).
 * I suppose I could connect it to any GPIO pin and "bit bang" frequencies,
 * but that would seriously affect tone accuracy.
 *
 * 2. How did you configure the GPIO pins?
 *
 * I configured PC1 as an output pin and set it to alternate function
 * #1 (T1OUT).  I also configured PC0 as an input pin, and tried both of its
 * alternate functions for trying to generate tones with the speaker connected
 * to it without luck.  GND is the more appropriate pin for that.
 *
 * 3. What is the highest frequency note you could play?  What's the lowest?
 * What limits the frequencies?
 *
 * Using the timer I can say frequency = syshz / (4 * reload * prescale).
 * So the theoretical maximum frequency is f = 18432000 / (4 * 1 * 1) = 4,608,000 Hz
 * The theoretical minimum frequency is f = 18432000 / (4 * 65535 * 4095) < 1 Hz
 * Neither of those frequencies are discernable by the human ear and are
 * almost certainly not reproducible by the little speaker in our lab kit.
 *
 * 4. How can you verify your notes are accurate (the right frequency)? Did
 * you verify your notes? Were your notes accurate?
 *
 * One could use a microphone or an oscilloscope to verify the notes produced
 * by the speaker (or the speaker signal) are accurate.  I used a free app on
 * my phone called 'gStrings' which told me the what frequency the speaker was
 * producing.  A random sampling of notes showed reasonable accuracy (within
 * 1 Hz).
 *
 * 5. Did you store your user defined tune in SRAM or FLASH memory? How did
 * you specify that? Why did you choose that? How can you verify that?
 *
 * I defined my tunes in flash memory.  I did that by adding the 'rom'
 * modifier/macro to the variable definitions.  I did it because I couldn't
 * store them in SRAM and have room to dynamically allocate memory for parsing
 * them at runtime.  When I changed the variables to be stored in flash ROM,
 * the linker output, which shows memory allocations, changed accordingly.
 * That, and the fact that my 'mallocs' stoped failing, was sufficient proof
 * for me.  However, I suppose I could print out the address that the string
 * pointers point to and compare that with the location of flash memory from
 * the product specification if I really needed to prove it.
 *
 * Of course, because it's read-only memory, I'm careful to copy the strings
 * into read-write memory when I need to modify them (as my parsing algorithm
 * does with strtok).
 */

#include <zneo.h>
#include <stdlib.h>
#include <stdio.h>
#include <sio.h>
#include "timer.h"
#include "ledarray.h"
#include "button.h"
#include "speaker.h"
#include "rtttl.h"
#include "utils.h"

static rom char *songdata1 = "Super Mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";
static rom char *songdata2 = "Zelda:d=4,o=5,b=125:a#,f.,8a#,16a#,16c6,16d6,16d#6,2f6,8p,8f6,16f.6,16f#6,16g#.6,2a#.6,16a#.6,16g#6,16f#.6,8g#.6,16f#.6,2f6,f6,8d#6,16d#6,16f6,2f#6,8f6,8d#6,8c#6,16c#6,16d#6,2f6,8d#6,8c#6,8c6,16c6,16d6,2e6,g6,8f6,16f,16f,8f,16f,16f,8f,16f,16f,8f,8f,a#,f.,8a#,16a#,16c6,16d6,16d#6,2f6,8p,8f6,16f.6,16f#6,16g#.6,2a#.6,c#7,c7,2a6,f6,2f#.6,a#6,a6,2f6,f6,2f#.6,a#6,a6,2f6,d6,2d#.6,f#6,f6,2c#6,a#,c6,16d6,2e6,g6,8f6,16f,16f,8f,16f,16f,8f,16f,16f,8f,8f";
static rom char *songdata3 = "Bach - Bouree:d=8,o=6,b=140:e,f#,4g,32p,16g,32g,32f#,e,4d#,e,f#,4b5,c#,d#,4e,d,c,4b5,a5,g5,4f#5,g5,a5,b5,a5,g5,f#5,4e5,32p,e,f#,4g,32p,16g,32g,32f#,e,4d#,e,f#,4b5,c#,d#,4e,d,c,4b5,a5,g5,32f#5,32e5,32f#5,4f#5,32p,g5,2g.5";

static rtttl_t song;
static volatile char *switch_to = NULL;

/*
 * Set new song data to be picked up by main loop and request song stop
 * Of course, this being a callback from a timer-interrupt-driven button press
 * the song doesn't actually stop playing until control returns back to the
 * main loop.
 */
void _replace_song(void *data) {
    char *songdata = (char *) data;

    switch_to = songdata;
    rtttl_stop(&song);
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
    button_t button1, button2, button3;
    int showing_help;

    _set_oscillator();

    init_uart(_UART0, _DEFFREQ, _DEFBAUD);
    clear_screen();

    timer_init(18432000);
    speaker_init(18432000);
    
    ledarray_init();
    ledarray_set_scroll_rate(40);
    
    /* put 'song' into a known state */
    rtttl_init(&song, songdata1);

    /* initialize the three physical buttons */
    button_init(&button1, &PDDD, &PDIN, (1 << 3));
    button_init(&button2, &PFDD, &PFIN, (1 << 6));
    button_init(&button3, &PFDD, &PFIN, (1 << 7));

    /* register callbacks */
    button_on_single_press(&button1, _replace_song, songdata1);
    button_on_single_press(&button2, _replace_song, songdata2);
    button_on_single_press(&button3, _replace_song, songdata3);
    button_on_double_press(&button1, _replace_song, NULL);
    button_on_double_press(&button2, _replace_song, NULL);
    button_on_double_press(&button3, _replace_song, NULL);

    showing_help = 0;

    while (1) {
        /* Anymore complicated and I'll need a finite state machine */
        if (switch_to) {
            showing_help = 0;
            rtttl_free(&song);
            if (rtttl_init(&song, switch_to)) {
                rtttl_play(&song);
            } else {
                switch_to = 0;
                showing_help = 1;
                ledarray_setstring("Failed to parse song...choose another.");
            }
        } else if (!showing_help) {
            showing_help = 1;
            ledarray_setstring("Press a button to play song...double press any to stop.");
        }
    }
}
