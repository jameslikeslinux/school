/*
 * utils.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120228
 * Lab:    2
 */

#ifndef UTILS_H
#define UTILS_H

/*
 * Prints ^L character to serial terminal (clears the screen)
 */
void clear_screen();

/*
 * Returns 'str' less spaces at the beginning and end
 */
char * trim_string(char *str);

/*
 * Shifts all the characters in 'str' one space to the left
 */
void delete_char(char *str);

/*
 * Delay for 'micros' us.  Not very accurate, but ballpark.
 */
void delay_micros(int micros);

#endif
