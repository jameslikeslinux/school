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