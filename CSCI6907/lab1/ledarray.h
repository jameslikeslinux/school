/*
 * ledarray.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120220
 * Lab:    1
 */

#ifndef LEDARRAY_H
#define LEDARRAY_H

/*
 * Initialize the set of LED panels
 */
void ledarray_init();

/*
 * Set scrolling behavior
 */
void ledarray_set_scroll_rate(int millies_per_char);
void ledarray_set_smooth_scroll(int s);
void ledarray_set_restart_scroll_on_update(int r);

/*
 * Display functions
 */
void ledarray_setstring(char *newstr);
void ledarray_setdecimal(int number);
void ledarray_sethexadecimal(int number);
void ledarray_setdouble(double number);

#endif
