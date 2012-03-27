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