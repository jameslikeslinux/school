/*
 * draw.h
 * James Lee <jlee23@umbc.edu>
 */

#ifndef DRAW_H
#define DRAW_H

#include <ri.h>

void drawCube(RtPoint from, RtPoint to);
void drawTable(RtFloat length, RtFloat width, RtFloat height);
void drawLamp(RtPoint position, RtFloat height);
void drawPencil();

#endif
