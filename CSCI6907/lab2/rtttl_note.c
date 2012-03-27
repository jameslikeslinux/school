/*
 * rtttl_note.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120228
 * Lab:    2
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "rtttl_note.h"
#include "speaker.h"
#include "ledarray.h"

/*
 * Convert note (assumed to be in the 4th octave) to its frequency
 * See: http://en.wikipedia.org/wiki/Piano_key_frequencies
 */
int _rtttl_note_to_frequency(char note, int sharp) {
    if (sharp) {
        switch (note) {
            case 'A': return 466;
            case 'C': return 277;
            case 'D': return 311;
            case 'F': return 370;
            case 'G': return 415;
            default:  break;
        }

        note++; /* b# is c; e# is f */
    }

    switch (note) {
        case 'A': return 440;
        case 'B': return 494;
        case 'C': return 261;
        case 'D': return 294;
        case 'E': return 330;
        case 'F': return 349;
        case 'G': return 392;
        default:  break;
    }

    return 0;
}

int rtttl_note_init(rtttl_note_t *rtttl_note, char *rawnote, int default_duration, int default_octave) {
    char note;
    int i, issharp, octave;

    /* store raw string so it can be printed as it's played */
    rtttl_note->rawnote = rawnote;

    rtttl_note->dotted = 0;

    /* check string begins with a number (the note duration) */
    if ((rtttl_note->duration = atoi(rawnote)) == 0) {
        rtttl_note->duration = default_duration;
    }

    /* then skip to the first non-digit */
    for (i = 0; i < strlen(rawnote) && isdigit(rawnote[i]); i++);

    if (i >= strlen(rawnote)) {
        printf("Ran out of characters.\n");
        return 0;
    }

    /* set default note settings */
    issharp = 0;
    octave = default_octave;

    /* parse the note name */
    note = toupper(rawnote[i]);
    if (!((note >= 'A' && note <= 'G') || note == 'P')) {
        printf("Parsed invalid note.\n");
        return 0;
    }

    if (++i >= strlen(rawnote)) {
        /* the rest of the note encoding is optional */
        goto FINISH_PARSE;
    }

    /* check for sharp symbol */
    if (rawnote[i] == '#') {
        issharp = 1;

        if (++i >= strlen(rawnote)) {
            goto FINISH_PARSE;
        }
    }

    /* check for dotted note */
    if (rawnote[i] == '.') {
        rtttl_note->dotted = 1;

        if (++i >= strlen(rawnote)) {
            goto FINISH_PARSE;
        }        
    }

    /* OH! How I'd love regex right about now... */

    /* check for octave */
    if (rawnote[i] >= '4' && rawnote[i] <= '7') {
        octave = atoi(&rawnote[i]);

        if (++i >= strlen(rawnote)) {
            goto FINISH_PARSE;
        }
    }

    /* some documentation says the dot comes after the octave */
    if (rawnote[i] == '.') {
        rtttl_note->dotted = 1;
        goto FINISH_PARSE;
    }

    printf("Should have finished parsing note but more characters remain.\n");
    return 0;

FINISH_PARSE:
    /* 
     * convert note information to frequency so we don't have to do it
     * while the song is playing.
     * octaves scale nicely...
     */
    rtttl_note->frequency = _rtttl_note_to_frequency(note, issharp) * pow(2, octave - 4);
    return 1;
}

void rtttl_note_play(rtttl_note_t *rtttl_note) {
    ledarray_setstring1(rtttl_note->rawnote, 0, 0);

    if (rtttl_note->frequency == 0) {
        speaker_stop();
    } else {
        speaker_play(rtttl_note->frequency);
    }
}

void rtttl_note_stop() {
    speaker_stop();
}

int rtttl_note_get_duration(rtttl_note_t *rtttl_note) {
    return rtttl_note->duration;
}

int rtttl_note_is_dotted(rtttl_note_t *rtttl_note) {
    return rtttl_note->dotted;
}
