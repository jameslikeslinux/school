/* set up and maintain view as window sizes change */

#include "view.h"

// Apple's annoying non-standard GL include location
#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int winWidth = 640, winHeight = 400;
float viewWidth = 300;

/*
 * this is called when window is created or resized
 *
 * scale projection to always see -viewWidth/2 to viewWidth/2 horizontally
 */
void 
reshape(int width, int height)
{
  /* distance in grid coordinates from edge to the center of the window */
  float xMax = viewWidth/2;
  float yMax = xMax * height / width;

  /* copy width and height into global variables so other callback
     functions can access them */
  winWidth = width;
  winHeight = height;

  /* this viewport makes a 1 to 1 mapping of physical pixels to GL
     "logical" pixels */
  glViewport(0, 0, (GLint) width, (GLint) height);

  /* adjust region of 3D space projected into this window */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, 1.0, 5.0, 500.0);

  /* switch back to "normal" transformation mode */
  glMatrixMode(GL_MODELVIEW);
}

