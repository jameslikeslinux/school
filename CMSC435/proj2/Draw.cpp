/*
 * Draw.cpp
 * James Lee <jlee23@umbc.edu>
 *
 * The functions contained in this file are called from the provided sample
 * code to make use of the C++ objects representing points, triangles, and
 * mountains.
 */

#include <cmath>
#include <GL/gl.h>
#include <GL/glut.h>
#include "Triangle.h"

typedef vector<Triangle*> Mountain;

static vector<Point*> allPoints;
static vector<Mountain> allMountains;
static int iteration = -1;
static int dispRange = 200;

// called by glut
extern "C" void drawMountain() {
	// if this is the first run
	if (iteration == -1) {
		// generate the two base triangles
		Point *lf = new Point(-100.0, -50.0, 100);
		Point *ln = new Point(-100.0, -50.0, -100.0);
		Point *rf = new Point(100.0, -50.0, 100.0);
		Point *rn = new Point(100.0, -50.0, -100.0);

		allPoints.push_back(lf);
		allPoints.push_back(ln);
		allPoints.push_back(rf);
		allPoints.push_back(rn);

		Mountain mountain;
		mountain.push_back(new Triangle(lf, rn, ln, &allPoints));
		mountain.push_back(new Triangle(lf, rn, rf, &allPoints));

		allMountains.push_back(mountain);
		iteration = 0;
	}

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
		if (!point->isModified() && x != -100.0 && x != 100.0 && z != -100.0 && z != 100.0) {
			// randomly generate a displacement with a 75% chance of being positive
			int displacement = rand() % (int) (dispRange * 0.75) - (dispRange * 0.25);
			// perform 1/f noise
			displacement /= pow(2.0, iteration);
			point->setPoint(x, y + displacement, z);
		}
	}
}

// called by glut input callback
extern "C" void subdivideUp() {
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
extern "C" void subdivideDown() {
	// there will always be a lower mountain stored
	if (iteration > 0)
		iteration--;
}

// called by glut input callback
extern "C" void increaseDisplacement() {
	// increase the range for future subdivisions
	dispRange *= 1.1;

	// and affect the current mountain, too
	for (unsigned int i = 0; i < allPoints.size(); i++) {
		Point *point = allPoints[i];
		float y = point->getPoint()[1];
		point->setPoint(point->getPoint()[0], ((y + 50.0) * 1.1 - 50.0), point->getPoint()[2]);
	}
}

// called by glut input callback
extern "C" void decreaseDisplacement() {
	dispRange *= (1.0 / 1.1);
	
	for (unsigned int i = 0; i < allPoints.size(); i++) {
		Point *point = allPoints[i];
		float y = point->getPoint()[1];
		point->setPoint(point->getPoint()[0], ((y + 50.0) * (1.0 / 1.1) - 50.0), point->getPoint()[2]);
	}
}
