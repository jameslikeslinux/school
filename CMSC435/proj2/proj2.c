/*
 * Simple GL example
 */

#include "draw.h"
#include "view.h"
#include "motion.h"

// Apple's annoying non-standard GL include location
#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>

/* initialize GLUT - windows and interaction */
void initGLUT(int *argcp, char *argv[])
{
  /* ask for a window at 0,0 with dimensions winWidth, winHeight */
  /* need color, depth (for 3D drawing) and double buffer (smooth display) */
  glutInit(argcp, argv);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(winWidth, winHeight);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow("Modeled Scene: 435 Assignment 2");

  /* set callback functions to be called by GLUT for drawing, window
     resize, keypress, mouse button press, and mouse movement */
  glutDisplayFunc(draw);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(key);
  glutMouseFunc(mousePress);
  glutMotionFunc(mouseDrag);
}

/* initialize OpenGL - rendering state */
void initGL()
{
  float lightdir[4] = {1,1,2,0};	/* homogeneous light position: directional if w=0 */
  float white[4] = {1,1,1,1}; 		/* color for light: glLightfv needs 4 components!*/
  float dim[4] = {.2,.2,.2,1};
 
  /* enable some GL features */
  glEnable(GL_DEPTH_TEST);		/* tell OpenGL to handle overlapping surfaces */
  glEnable(GL_COLOR_MATERIAL);		/* map glColor to surface colors used by lighting */
  glEnable(GL_NORMALIZE);		/* tell GL to normalize normals so we don't have to */

  /* set up one light for both directional and ambient */
  glLightfv(GL_LIGHT0, GL_AMBIENT, dim);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
  glLightfv(GL_LIGHT0, GL_POSITION, lightdir);
  glEnable(GL_LIGHT0);			/* turn on this light */
  glEnable(GL_LIGHTING);		/* turn on use of lighting in general */
}

int main(int argc, char *argv[])
{
  /* set up GLUT and OpenGL */
  initGLUT(&argc, argv);
  initGL();
  
  /* let glut take over, it goes into a loop, checking for input and
     calling the input callbacks, then seeing if we need to draw and
     calling the draw callback, ad infinitum */
  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}
