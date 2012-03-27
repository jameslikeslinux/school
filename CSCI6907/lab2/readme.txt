Author: James Lee
Email:  jameslee@gwu.edu
Class:  CSCI 6907
Date:   20120228
Lab:    2

Description: This lab uses a timer to generate accurate tones from a speaker
connected across T1OUT (PC1) and GND.  It also implements an RTTTL parser
along with three RTTTL tunes that can be parsed and played through the
speaker at the press of one of the three buttons.

Other files: button.{c,h} led.{c,h} ledarray.{c,h} matrix.h timer.{c,h}
             rtttl.{c,h} rtttl_note.{c,h} utils.{c,h}

Problems: In order to get sound to play from the speaker, I had to connect
one speaker pin to the T1OUT pin, and the other to GND, not PC0/T1IN as
specified in the lab.  It seems to me that that shouldn't work/be necessary
in any case.  PC0/T1IN is a high-impedance digital logic input so you
wouldn't get the current flow necessary for sound, plus there is no
functional advantage to having the speaker sinking to a timer input.  It's a
waste of a GPIO pin as well.  GND is the more suitable sink for the speaker.

Enhancements: I added a basic user interface that prompts the user to press
a button, displays the name of the song being played, and loops the song
until a button is double-pressed.

Questions:

1. Could you have used other IO pins.  Why/why not?

As I wrote, I did have to sink the speaker to GND rather than a GPIO pin.
For the speaker source, I could have connected it to another timer output,
such as PC7/T2OUT, or PA1/T0OUT (if T0 wasn't being used for event control).
I suppose I could connect it to any GPIO pin and "bit bang" frequencies,
but that would seriously affect tone accuracy.

2. How did you configure the GPIO pins?

I configured PC1 as an output pin and set it to alternate function
#1 (T1OUT).  I also configured PC0 as an input pin, and tried both of its
alternate functions for trying to generate tones with the speaker connected
to it without luck.  GND is the more appropriate pin for that.

3. What is the highest frequency note you could play?  What's the lowest?
What limits the frequencies?

Using the timer I can say frequency = syshz / (4 * reload * prescale).
So the theoretical maximum frequency is f = 18432000 / (4 * 1 * 1) = 4,608,000 Hz
The theoretical minimum frequency is f = 18432000 / (4 * 65535 * 4095) < 1 Hz
Neither of those frequencies are discernable by the human ear and are
almost certainly not reproducible by the little speaker in our lab kit.

4. How can you verify your notes are accurate (the right frequency)? Did
you verify your notes? Were your notes accurate?

One could use a microphone or an oscilloscope to verify the notes produced
by the speaker (or the speaker signal) are accurate.  I used a free app on
my phone called 'gStrings' which told me the what frequency the speaker was
producing.  A random sampling of notes showed reasonable accuracy (within
1 Hz).

5. Did you store your user defined tune in SRAM or FLASH memory? How did
you specify that? Why did you choose that? How can you verify that?

I defined my tunes in flash memory.  I did that by adding the 'rom'
modifier/macro to the variable definitions.  I did it because I couldn't
store them in SRAM and have room to dynamically allocate memory for parsing
them at runtime.  When I changed the variables to be stored in flash ROM,
the linker output, which shows memory allocations, changed accordingly.
That, and the fact that my 'mallocs' stoped failing, was sufficient proof
for me.  However, I suppose I could print out the address that the string
pointers point to and compare that with the location of flash memory from
the product specification if I really needed to prove it.

Of course, because it's read-only memory, I'm careful to copy the strings
into read-write memory when I need to modify them (as my parsing algorithm
does with strtok).

