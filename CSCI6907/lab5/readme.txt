 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120402
 * Lab:    5
 *
 * Description: This lab implements a 16 Kb I2C memory module (24LC16B) and
 * a 4 Kb SPI memory module (25LC040A) from Microchip.  It adds two new
 * commands to the CLI called 'i2c' and 'spi' to fill, dump, and erase, and
 * write strings to the memory chips.
 *
 * SEE 'schematic.png' FOR THE SCHEMATIC
 *
 * Other files: button.{c,h} cli.{c,h} i2c.{c,h} i2c_mem.{c,h} led.{c,h}
 * ledarray.{c,h} matrix.h queue.{c,h} spi.{c,h} spi_mem.{c,h} timer.{c,h}
 * uart.{c,h} utils.{c,h}
 *
 * Problems: No major problems, however I2C was tricky.
 *
 * Enhancements: I also implemented the 'write' commands to write text to
 * locations in memory, the 'erase' command to clear memory, and I modified
 * the dump output to show ASCII representations of the memory.
 *
 * Questions:
 *
 * 1. What is the full part number for each device?
 *
 * 16K I2C Serial EEPROM: 24LC16B (I temp range; P package)
 * 4K SPI Bus Serial EEPROM: 25LC040A (I temp range, P package)
 *
 * 2. Schematic
 *
 * See 'schematic.png'.
 *
 * 3. Did you have any problems finding the datasheets or getting the
 * necessary information?
 *
 * No.  The datasheets are among the first few results when searching
 * on Google for the part numbers.
 *
 * 4. What techniques did you use to help debug your program?
 *
 * When I was having trouble getting the I2C read function to work, I
 * simplified everything until it did work.  I slowed down the I2C clock,
 * got the simpler write function to work, got a simple sequential read
 * work, and finally, the random read.  An oscilloscope probably would have
 * helped, but I do not have convenient access to one.
 *
 * 5. Did you need any pull up resistors?  Why?
 *
 * Both the SCK and SDA lines of the I2C bus are open-drain, so they do
 * need to be pulled high independently.  I used the ZNEO's internal pull-ups
 * rather than an external resistor.