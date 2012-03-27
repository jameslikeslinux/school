/*
 * speaker.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120228
 * Lab:    2
 */

#ifndef SPEAKER_H
#define SPEAKER_H

/*
 * Initialize the timer/speaker output
 *
 * hz - the system clock rate
 */
void speaker_init(int hz);

/*
 * Configure the timer/spearker to output at 'frequency' Hz
 */
void speaker_play(int frequency);

/*
 * Configure the timer/speaker to be silent
 */
void speaker_stop();

#endif
