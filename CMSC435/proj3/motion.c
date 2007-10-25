/* handle mouse motion and rotating the object */

#include "motion.h"
#include "view.h"
#include "Globals.h"

// Apple's annoying non-standard GL include location
#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>

int buttonPressed;		/* which mouse button was pressed? */
int oldX, oldY;			/* where was our last position? */

/*
 * called when a mouse button is pressed. 
 * 
 * Remember where we were, and what mouse button it was.
 */
void
mousePress(int b, int state, int x, int y)
{
  buttonPressed = b;
  oldX = x;
  oldY = y;
}

/*
 * called when the mouse moves
 *
 * use difference between oldX,oldY and x,y to define a rotation
 */
void
mouseDrag(int x, int y)
{
  float dx = x - oldX;		/* record differences from last position */
  float dy = y - oldY;

  dx /= 2;
  dy /= 10;

  oldX = x;			/* update stored position */
  oldY = y;

  /* rotation angle, scaled so 1/2 window width = about 90 degrees */
  float angle = 180 * dx / winWidth;

  heading -= angle;
  if (heading > 360.0)
    heading -= 360.0;
  if (heading < 0.0)
    heading += 360.0;

  
  carx += dy * sin(heading * PI / 180);
  carz -= dy * cos(heading * PI / 180);

  /* tell GLUT that something has changed and we must redraw */
  glutPostRedisplay();
}
