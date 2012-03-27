/*
 * rtttl.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120228
 * Lab:    2
 */

#ifndef RTTTL_H
#define RTTTL_H

#include "rtttl_note.h"

typedef struct {
    /*
     * PRIVATE
     */
    char *rawrtttl;
    char *name;
    int default_duration;
    int tempo;
    int numnotes;
    rtttl_note_t *notes;
    volatile int stopped;
} rtttl_t;

/*
 * Initialize a rtttl object.  'rawrtttl' will be copied to an internal
 * buffer and parsed.
 *
 * rawrtttl - A ringtone encoded in RTTTL (will be copied; not modified)
 * returns - 0 if parse failure; 1 if parse success
 */
int rtttl_init(rtttl_t *rtttl, /* const */ char *rawrtttl);

/*
 * Frees internal buffers and data strutures used to parse/store the RTTTL
 */
void rtttl_free(rtttl_t *rtttl);

/*
 * Plays the parsed, valid RTTTL once.  Blocks until completion.
 */
void rtttl_play(rtttl_t *rtttl);

/*
 * Stop playback and return from 'rtttl_play'
 */
void rtttl_stop(rtttl_t *rtttl);

#endif
