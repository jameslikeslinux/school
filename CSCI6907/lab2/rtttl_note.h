/*
 * rtttl_note.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120228
 * Lab:    2
 */

#ifndef RTTTL_NOTE_H
#define RTTTL_NOTE_H

typedef struct {
    /*
     * PRIVATE
     */
    char *rawnote;  /* the RTTTL representation of the note */
    int frequency;  /* the actual frequency in hz */
    int duration;   /* musically (i.e. 'half' -> 2; 'quarter' -> 4; etc) */
    int dotted;     /* whether the note is dotted */
} rtttl_note_t;

/*
 * Initializes an rtttl_note_t object by parsing a raw RTTTL note string
 *
 * rawnote - A valid RTTTL note string
 * default_duration - The length of a note if not specified in 'rawnote'
 * default_octave - The scale of the note if not specified in 'rawnote'
 * returns - 0 if parse failure; 1 if parse success
 */
int rtttl_note_init(rtttl_note_t *rtttl_note, /* const */ char *rawnote, int default_duration, int default_octave);

/*
 * Configures the speaker to output at 'frequency' or silent if
 * 'frequency' is 0.
 *
 * Does not block.  Tempo is not the note's responsibility so duration
 * must be controlled at a higher layer.
 */
void rtttl_note_play(rtttl_note_t *rtttl_note);

/*
 * Configure the speaker to be silent
 */
void rtttl_note_stop();

/*
 * GETTERS
 */
int rtttl_note_get_duration(rtttl_note_t *rtttl_note);
int rtttl_note_is_dotted(rtttl_note_t *rtttl_note);

#endif
