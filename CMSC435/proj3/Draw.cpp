/*
 * Draw.cpp
 * James Lee <jlee23@umbc.edu>
 *
 * The functions contained in this file are called from the provided sample
 * code to make use of the C++ objects representing points, triangles, and
 * mountains.
 */

#include <cmath>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>
#include "Draw.h"
#include "Triangle.h"
#include "Globals.h"

typedef vector<Triangle*> Mountain;

static vector<Point*> allPoints;
static vector<Mountain> allMountains;
static int iteration = -1;
static int dispRange = 200;

// called by glut
void drawMountain() {
	// if this is the first run
	if (iteration == -1) {
		// generate the two base triangles
		Point *lf = new Point(-MOUNTAIN_CORNER, -MOUNTAIN_BASE,  MOUNTAIN_CORNER);
		Point *ln = new Point(-MOUNTAIN_CORNER, -MOUNTAIN_BASE, -MOUNTAIN_CORNER);
		Point *rf = new Point(MOUNTAIN_CORNER, -MOUNTAIN_BASE, MOUNTAIN_CORNER);
		Point *rn = new Point(MOUNTAIN_CORNER, -MOUNTAIN_BASE, -MOUNTAIN_CORNER);

		allPoints.push_back(lf);
		allPoints.push_back(ln);
		allPoints.push_back(rf);
		allPoints.push_back(rn);

		Mountain mountain;
		mountain.push_back(new Triangle(lf, rn, ln, &allPoints));
		mountain.push_back(new Triangle(lf, rn, rf, &allPoints));

		allMountains.push_back(mountain);
		iteration = 0;

		for (int i = 0; i < divisions; i++)
			subdivideUp();
		
		for (int i = 0; i < 2; i++)
			increaseDisplacement();

		// normalize everything
		for (unsigned int i = 0; i < allMountains[iteration].size(); i++)
			allMountains[iteration][i]->normalize();

		for (unsigned int i = 0; i < allPoints.size(); i++)
			allPoints[i]->normalize();
	}

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cary = -MOUNTAIN_BASE;
	float up[] = {0.0, 1.0, 0.0};

	if (carx >= -MOUNTAIN_CORNER && carx <= MOUNTAIN_CORNER &&
	    carz >= -MOUNTAIN_CORNER && carz <= MOUNTAIN_CORNER) {
		float hitPos[3];
		bool hit = false;
		unsigned int i;

		for (i = 0; i < allMountains[iteration].size() && !hit; i++)
			hit = allMountains[iteration][i]->livesWithinXZ(carx, carz, hitPos);

		if (hit && hitPos[1] > -MOUNTAIN_BASE) {
			cary = hitPos[1];
			allMountains[iteration][i - 1]->getInterpolatedNormalAtLastKnownPoint(up);
		}
	}

	glLoadIdentity();
	glRotatef(-asin(up[2] / 1.0) * 180 / PI, cos(heading * PI / 180), 0.0, -sin(heading * PI / 180));
	glRotatef(asin(up[0] / 1.0) * 180 / PI, sin(heading * PI / 180), 0.0, cos(heading * PI / 180));
	glRotatef(heading, 0.0, 1.0, 0.0);
	glTranslatef(-carx + (5.0 * sin(heading * PI / 180)), -cary - 15.0, -carz - (5.0 * cos(heading * PI / 180)));


	// draw the ocean
	glBegin(GL_QUADS);
	glColor3f(0.1, 0.3, 1.0);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-10000.0, -49.9, 10000.0);
	glVertex3f(-10000.0, -49.9, -10000.0);
	glVertex3f(10000.0, -49.9, -10000.0);
	glVertex3f(10000.0, -49.9, 10000.0);
	glEnd();

	// draw all the triangles
	glBegin(GL_TRIANGLES);
	glColor3f(1.0, 1.0, 1.0);
	for (unsigned int i = 0; i < allMountains[iteration].size(); i++)
		allMountains[iteration][i]->draw();
	glEnd();
/*
	glBegin(GL_LINES);
	for (unsigned int i = 0; i < allPoints.size(); i++) {
		const float *point = allPoints[i]->getPoint();
		const float *normal = allPoints[i]->getNormal();
		glColor3f(1.0, 0.0, 0.0);
		glVertex3fv(point);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(point[0] + normal[0] * 5, point[1] + normal[1] * 5, point[2] + normal[2] * 5);
	}
	
	for (unsigned int i = 0; i < allMountains[iteration].size(); i++)
		allMountains[iteration][i]->drawNormal();
	glEnd();
*/
	glutSwapBuffers();
}

static void displace() {
	for (unsigned int i = 0; i < allPoints.size(); i++) {
		Point *point = allPoints[i];
		float x = point->getPoint()[0];
		float y = point->getPoint()[1];
		float z = point->getPoint()[2];

		// if the point is newly created (such as by a subdivision)
		// and it's not an edge point
		if (!point->isModified() && x != -MOUNTAIN_CORNER && x != MOUNTAIN_CORNER && z != -MOUNTAIN_CORNER && z != MOUNTAIN_CORNER) {
			// randomly generate a displacement with a 75% chance of being positive
			int displacement = rand() % (int) (dispRange * 0.75) - (dispRange * 0.25);
			// perform 1/f noise
			displacement /= pow(2.0, iteration);
			point->setPoint(x, y + displacement, z);
		}
	}
}

// called by glut input callback
void subdivideUp() {
	// if the "mountain" is already stored.
	if (iteration < (int) allMountains.size() - 1) {
		iteration++;
		return;
	}

	// otherwise generate a new one
	Mountain mountain;
	for (unsigned int i = 0; i < allMountains[iteration].size(); i++) {
		Mountain partialMountain = allMountains[iteration][i]->subdivide();
		mountain.insert(mountain.end(), partialMountain.begin(), partialMountain.end());
	}

	allMountains.push_back(mountain);
	iteration++;

	displace();
}

// called by glut input callback
void subdivideDown() {
	// there will always be a lower mountain stored
	if (iteration > 0)
		iteration--;
}

// called by glut input callback
void increaseDisplacement() {
	// increase the range for future subdivisions
	dispRange *= 1.1;

	// and affect the current mountain, too
	for (unsigned int i = 0; i < allPoints.size(); i++) {
		Point *point = allPoints[i];
		float y = point->getPoint()[1];
		point->setPoint(point->getPoint()[0], ((y + MOUNTAIN_BASE) * 1.1 - MOUNTAIN_BASE), point->getPoint()[2]);
	}
}

// called by glut input callback
void decreaseDisplacement() {
	dispRange *= (1.0 / 1.1);
	
	for (unsigned int i = 0; i < allPoints.size(); i++) {
		Point *point = allPoints[i];
		float y = point->getPoint()[1];
		point->setPoint(point->getPoint()[0], ((y + MOUNTAIN_BASE) * (1.0 / 1.1) - MOUNTAIN_BASE), point->getPoint()[2]);
	}
}
