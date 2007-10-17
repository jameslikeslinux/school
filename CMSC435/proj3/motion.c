/* handle mouse motion and rotating the object */

#include "motion.h"
#include "view.h"

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

  oldX = x;			/* update stored position */
  oldY = y;

  if (buttonPressed == GLUT_LEFT_BUTTON) {
    /* rotation angle, scaled so 1/2 window width = about 90 degrees */
    float angle = 180 * sqrt(dx*dx + dy*dy) / winWidth;

    /* save current rotation state */
    float matrix[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);


    /* 
       From the starting state, rotate around axis perpendicular to
       mouse motion, where the window x axis is aligned with the 3D
       view x axis, while the window y axis is aligned with the 3D
       view z axis. Find perpendicular using rule that (y,-x) is
       perpendicular to (x,y)
    */
    glLoadIdentity();
    glRotatef(angle, dy,dx,0);

    /* reapply other rotations so far */
    glMultMatrixf(matrix);

    /* tell GLUT that something has changed and we must redraw */
    glutPostRedisplay();
  }
}
