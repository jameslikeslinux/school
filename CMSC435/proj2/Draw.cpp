#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>
#include "Triangle.h"


typedef vector<Triangle*> Mountain;

static vector<Point*> allPoints;
static vector<Mountain> allMountains;

static int iteration = -1;
static int dispRange = 200;
static int seed;

extern "C" void drawMountain() {
	if (iteration == -1) {
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

	glBegin(GL_QUADS);
	glColor3f(0.0, 0.0, 1.0);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-1000.0, -49.9, 1000.0);
	glVertex3f(-1000.0, -49.9, -1000.0);
	glVertex3f(1000.0, -49.9, -1000.0);
	glVertex3f(1000.0, -49.9, 1000.0);
	glEnd();

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

		if (!point->isModified() && x != -100.0 && x != 100.0 && z != -100.0 && z != 100.0) {
			int displacement = rand() % (int) (dispRange * 0.75) - (dispRange * 0.25);
			displacement /= iteration; 
			point->setPoint(x, y + displacement, z);
		}
	}
}

extern "C" void subdivideUp() {
	if (iteration < (int) allMountains.size() - 1) {
		iteration++;
		return;
	}

	Mountain mountain;
	for (unsigned int i = 0; i < allMountains[iteration].size(); i++) {
		Mountain partialMountain = allMountains[iteration][i]->subdivide();
		mountain.insert(mountain.end(), partialMountain.begin(), partialMountain.end());
	}

	allMountains.push_back(mountain);
	iteration++;

	displace();
}

extern "C" void subdivideDown() {
	if (iteration > 0)
		iteration--;
}

extern "C" void increaseDisplacement() {
	dispRange *= 1.1;

	for (unsigned int i = 0; i < allPoints.size(); i++) {
		Point *point = allPoints[i];
		float y = point->getPoint()[1];
		point->setPoint(point->getPoint()[0], ((y + 50.0) * 1.1 - 50.0), point->getPoint()[2]);
	}
}

extern "C" void decreaseDisplacement() {
	dispRange *= (1.0 / 1.1);
	
	for (unsigned int i = 0; i < allPoints.size(); i++) {
		Point *point = allPoints[i];
		float y = point->getPoint()[1];
		point->setPoint(point->getPoint()[0], ((y + 50.0) * (1.0 / 1.1) - 50.0), point->getPoint()[2]);
	}
}

extern "C" void setSeed(int newSeed) {
	seed = newSeed;
	srand(seed);
}
