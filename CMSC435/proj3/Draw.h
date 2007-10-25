/*
 * draw.h
 * James Lee <jlee23@umbc.edu>
 *
 * Prototypes for functions in Draw.cpp
 */
#ifndef DRAW_H
#define DRAW_H

#ifdef __cplusplus
extern "C" {
#endif
	void drawMountain();
	void subdivideUp();
	void subdivideDown();
	void increaseDisplacement();
	void decreaseDisplacement();
#ifdef __cplusplus
}
#endif

#endif
