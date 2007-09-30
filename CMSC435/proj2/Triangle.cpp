/*
 * Triangle.cpp
 * James Lee <jlee23@umbc.edu>
 *
 * Implementation for Triangle class.
 * A lot of this is stupidly verbose and probably overcomplicated.
 * Sorry.
 */

#include <iostream>
#include <cmath>
#include <GL/gl.h>
#include "Triangle.h"

// Helper function to return a point between two points.
// There is no error checking.
// The pointer returned is to dynamically allocated memory.
static Point* averagePoint(const Point *point1, const Point *point2) {
	float x = (point1->getPoint()[0] + point2->getPoint()[0]) / 2;
	float y = (point1->getPoint()[1] + point2->getPoint()[1]) / 2;
	float z = (point1->getPoint()[2] + point2->getPoint()[2]) / 2;

	return new Point(x, y, z);
}

// Pretty much makes a set out of a vector, making sure only one copy of a
// point exists in a vector.
static Point* getPoint(vector<Point*> *allPoints, Point *point) {
	for (unsigned int i = 0; i < allPoints->size(); i++) {
		if (*allPoints->at(i) == *point) {
			delete point;	// meh.
			return allPoints->at(i);
		}
	}

	allPoints->push_back(point);
	return point;
}

Triangle::Triangle(Point *point1, Point *point2, Point *point3, vector<Point*> *allPoints) {
	points[0] = point1;
	points[1] = point2;
	points[2] = point3;
	this->allPoints = allPoints;
}

vector<Triangle*> Triangle::subdivide() const {
	Point *point1 = getPoint(allPoints, averagePoint(points[0], points[1]));
	Point *point2 = getPoint(allPoints, averagePoint(points[0], points[2]));
	Point *point3 = getPoint(allPoints, averagePoint(points[1], points[2]));

	vector<Triangle*> triangles;
	triangles.push_back(new Triangle(points[0], point1, point2, allPoints));
	triangles.push_back(new Triangle(points[1], point1, point3, allPoints));
	triangles.push_back(new Triangle(points[2], point2, point3, allPoints));
	triangles.push_back(new Triangle(point1, point2, point3, allPoints));

	return triangles;
}

void Triangle::draw() {
	// The points are messed with outside of this object so we have to
	// renormalize since we don't know when the points change.
	normalize();

	glNormal3fv(normal);
	points[0]->draw();
	points[1]->draw();
	points[2]->draw();
}

// This is a pretty standard normalization function taken from some of my old
// code.
void Triangle::normalize() {
	float a[3], b[3];

	a[0] = points[0]->getPoint()[0] - points[1]->getPoint()[0];
	a[1] = points[0]->getPoint()[1] - points[1]->getPoint()[1];
	a[2] = points[0]->getPoint()[2] - points[1]->getPoint()[2];
	
	b[0] = points[1]->getPoint()[0] - points[2]->getPoint()[0];
	b[1] = points[1]->getPoint()[1] - points[2]->getPoint()[1];
	b[2] = points[1]->getPoint()[2] - points[2]->getPoint()[2];

	normal[0] = a[1] * b[2] - a[2] * b[1];
	normal[1] = a[2] * b[0] - a[0] * b[2];
	normal[2] = a[0] * b[1] - a[1] * b[0];

	float len = (float) sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);

	if (len == 0.0)
		len = 1.0;
	
	normal[0] /= len;
	normal[1] /= len;
	normal[2] /= len;
}
