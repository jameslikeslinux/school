#include <iostream>
#include "Triangle.h"

typedef vector<Triangle*> Mountain;

static vector<Point*> allPoints;
static vector<Mountain> allMountains;
static int currentMountain = -1;

extern "C" void drawMountain() {
	if (currentMountain == -1) {
		Point *lf = new Point(-100.0, 0.0, 100);
		Point *ln = new Point(-100.0, 0.0, -100.0);
		Point *rf = new Point(100.0, 0.0, 100.0);
		Point *rn = new Point(100.0, 0.0, -100.0);

		allPoints.push_back(lf);
		allPoints.push_back(ln);
		allPoints.push_back(rf);
		allPoints.push_back(rn);

		Mountain mountain;
		mountain.push_back(new Triangle(lf, rn, ln, &allPoints));
		mountain.push_back(new Triangle(lf, rn, rf, &allPoints));

		allMountains.push_back(mountain);
		currentMountain = 0;
	}
	
	for (unsigned int i = 0; i < allMountains[currentMountain].size(); i++) {
		cout << "Triangle " << i << ":\n";
		allMountains[currentMountain][i]->draw();
		cout << endl;
	}
}

extern "C" void subdivideUp() {
	if (currentMountain < (int) allMountains.size() - 1) {
		currentMountain++;
		return;
	}

	Mountain mountain;
	for (unsigned int i = 0; i < allMountains[currentMountain].size(); i++) {
		Mountain partialMountain = allMountains[currentMountain][i]->subdivide();
		mountain.insert(mountain.end(), partialMountain.begin(), partialMountain.end());
	}

	allMountains.push_back(mountain);
	currentMountain++;
}

extern "C" void subdivideDown() {
	if (currentMountain > 0)
		currentMountain--;
}
