#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef DEFINE_GLOBALS
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN int divisions;
EXTERN float carx, cary, carz, heading;

#define MOUNTAIN_CORNER 100.0
#define MOUNTAIN_BASE 50.0
#define PI 3.14159265

#endif
