/*
 * Triangle.h
 * James Lee <jlee23@umbc.edu>
 *
 * Class definition of a triangle, which is made up of three points.
 */

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vector>
using namespace std;

#include "Point.h"

class Triangle {
	public:
		// These points had better be in the vector of all points!
		Triangle(Point *point1, Point *point2, Point *point3, vector<Point*> *allPoints);

		// Any triangles and points returned by this function are
		// dynamically allocated.
		vector<Triangle*> subdivide() const;

		// This expects to be called only from the glut draw callback.
		// Don't call it from elsewhere!
		void draw();
		void drawNormal();

		// Adds normal for triangle to all of its points
		void normalize();

		bool livesWithinXZ(float x, float z, float *hitPos);

		// Interpolates based on the barycentric coordinates
		// stored by the last successful call of livesWithinXZ
		void getInterpolatedNormalAtLastKnownPoint(float *intNormal);

	private:
		Point *points[3];
		float normal[3];
		float u, v, w;
		vector<Point*> *allPoints;
};

#endif
