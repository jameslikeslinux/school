/*
 * button.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120220
 * Lab:    1
 */

#ifndef BUTTON_H
#define BUTTON_H

typedef struct {
    unsigned char volatile near *port;
    int bit;
    void (*single_press_callback)(void *);
    void *single_press_data;
    void (*double_press_callback)(void *);
    void *double_press_data;
    int isdepressed;
    long lastpressed;
    int timespressed;
} button_t;

void button_init(button_t *button, unsigned char volatile near *portdd, unsigned char volatile near *port, int bit);
void button_on_single_press(button_t *button, void (*callback)(void *), void *data);
void button_on_double_press(button_t *button, void (*callback)(void *), void *data);

#endif
