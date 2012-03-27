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