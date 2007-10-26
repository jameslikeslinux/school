/*
 * Globals.h
 * James Lee <jlee23@umbc.edu>
 *
 * Global variables and preprocessor constants
 * Hey, they're not that bad!
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef DEFINE_GLOBALS
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN int divisions;			// set by the main function
EXTERN float carx, cary, carz, heading;	// used by the mouse and draw callbacks

#define MOUNTAIN_CORNER 100.0
#define MOUNTAIN_BASE 50.0
#define PI 3.14159265

#endif
