/*
 * ledarray.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120228
 * Lab:    2
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

/* 
 * Additional settings around 'ledarray_setstring'
 *
 * scroll - Disable to force long strings not to scroll
 * wait - Block until entire string has been displayed or 3 seconds if
 *        string is short
 */
void ledarray_setstring1(char *newstr, int scroll, int wait);

/*
 * Return from 'ledarray_setstring1' if it is still waiting
 */
void ledarray_stopwaiting();

#endif
