/*
 * rtttl.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120228
 * Lab:    2
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "rtttl.h"
#include "timer.h"
#include "ledarray.h"
#include "utils.h"

int rtttl_init(rtttl_t *rtttl, char *rawrtttl) {
    char *control, *notes, *token;
    char ctlname;
    int ctlvalue, default_octave, i;

    /* allocate memory and copy raw string */
    rtttl->rawrtttl = (char *) malloc((strlen(rawrtttl) + 1) * sizeof(char));
    strcpy(rtttl->rawrtttl, rawrtttl);

    /* extract name */
    token = strtok(rtttl->rawrtttl, ":");
    
    if (token == NULL) {
        /* parse failed */
        return 0;
    }

    rtttl->name = trim_string(token);
    printf("Name: '%s'\n", rtttl->name);

    /* extract control string */
    token = strtok(NULL, ":");

    if (token == NULL) {
        /* parse failed */
        return 0;
    }

    control = trim_string(token);
    printf("Control: '%s'\n", control);

    /* extract notes */
    token = strtok(NULL, ":");

    if (token == NULL) {
        /* parse failed */
        return 0;
    }

    notes = trim_string(token);
    printf("Notes: '%s'\n", notes);

    /* if we haven't reached the end of the string, there's a problem */
    if (strtok(NULL, ":")) {
        return 0;
    }



    /*
     * PARSE CONTROL SECTION
     */

    /* set defaults */
    rtttl->default_duration = 4;
    default_octave = 6;
    rtttl->tempo = 63;

    token = strtok(control, ",");
    while (token != NULL) {
        token = trim_string(token);

        if (sscanf(token, "%c=%d", &ctlname, &ctlvalue) == 2) {
            switch (tolower(ctlname)) {
                case 'd':
                    rtttl->default_duration = ctlvalue;
                    break;

                case 'o':
                    default_octave = ctlvalue;
                    break;

                case 'b':
                    rtttl->tempo = ctlvalue;
                    break;

                default:
                    /* ignore */
                    break;
            }
        }

        token = strtok(NULL, ",");
    }

    printf("D: %d\nO: %d\nB: %d\n", rtttl->default_duration, default_octave, rtttl->tempo);


    /*
     * PARSE NOTES
     */

    /* first, count the number of notes expected */
    rtttl->numnotes = 0;
    for (i = 0; i < strlen(notes); i++) {
        if (notes[i] == ',') {
            rtttl->numnotes++;
        }
    }

    if (rtttl->numnotes > 0) {
        /* there's always one more note than there is number of commas */
        rtttl->numnotes++;
    } else {
        printf("No notes found.\n");
        return 0;
    }

    /* allocate memory for note objects */
    rtttl->notes = (rtttl_note_t *) malloc(rtttl->numnotes * sizeof(rtttl_note_t));
    if (rtttl->notes == NULL) {
        printf("Could not allocate memory for notes.\n");
        return 0;
    }

    i = 0;
    token = strtok(notes, ",");
    while (token != NULL) {
        token = trim_string(token);

        if (!rtttl_note_init(&rtttl->notes[i], token, rtttl->default_duration, default_octave)) {
            /* note parse failure -> song parse failure */
            rtttl_free(rtttl);
            return 0;
        }

        i++;
        token = strtok(NULL, ",");
    }

    return 1;
}

void rtttl_free(rtttl_t *rtttl) {
    free(rtttl->notes);
    rtttl->notes = NULL;

    free(rtttl->rawrtttl);
    rtttl->rawrtttl = NULL;
}

void rtttl_play(rtttl_t *rtttl) {
    int i, millis_per_beat, millis_to_wait;

    rtttl->stopped = 0;

    /* 60000 ms per minute; tempo in beats per minute */
    millis_per_beat = 60000 / rtttl->tempo;

    /* show name of song on LEDs and wait until it has scrolled off */
    ledarray_setstring1(rtttl->name, 1, 1);

    /* play every note in order, unless told to stop */
    for (i = 0; i < rtttl->numnotes && !rtttl->stopped; i++) {
        rtttl_note_play(&rtttl->notes[i]);

        /* Unlike actual music; in rtttl I guess the quarter note always gets the beat */
        /* millis_to_wait = rtttl->default_duration * millis_per_beat / rtttl_note_get_duration(&rtttl->notes[i]); */
        millis_to_wait = 4 * millis_per_beat / rtttl_note_get_duration(&rtttl->notes[i]);

        if (rtttl_note_is_dotted(&rtttl->notes[i])) {
            millis_to_wait *= 1.5;
        }

        timer_sleep(millis_to_wait);
    }

    rtttl_note_stop();
    ledarray_setstring("");
}

void rtttl_stop(rtttl_t *rtttl) {
    /* break out of 'play' for loop */
    rtttl->stopped = 1;

    /* break out of any timer_sleep statements */
    ledarray_stopwaiting();

    /* break out of any waiting ledarray_setstring1 statements */
    timer_wakeup();
}
