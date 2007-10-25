/*
 * Triangle.cpp
 * James Lee <jlee23@umbc.edu>
 *
 * Implementation for Triangle class.
 * A lot of this is stupidly verbose and probably overcomplicated.
 * Sorry.
 */

#include <cmath>
#include <GL/gl.h>
#include "Triangle.h"
#include "Globals.h"

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
	points[0]->draw();
	points[1]->draw();
	points[2]->draw();
}

void Triangle::drawNormal() {
	float point[3];
	point[0] = (points[0]->getPoint()[0] + points[1]->getPoint()[0] + points[2]->getPoint()[0]) / 3;
	point[1] = (points[0]->getPoint()[1] + points[1]->getPoint()[1] + points[2]->getPoint()[1]) / 3;
	point[2] = (points[0]->getPoint()[2] + points[1]->getPoint()[2] + points[2]->getPoint()[2]) / 3;

	glColor3f(1.0, 1.0, 0.0);
	glVertex3fv(point);
	glVertex3f(point[0] + normal[0] * 5, point[1] + normal[1] * 5, point[2] + normal[2] * 5);
}

// This is a pretty standard normalization function taken from some of my old
// code.
void Triangle::normalize() {
	float a[3], b[3];

	a[0] = points[0]->getPoint()[0] - points[1]->getPoint()[0];
	a[1] = points[0]->getPoint()[1] - points[1]->getPoint()[1];
	a[2] = points[0]->getPoint()[2] - points[1]->getPoint()[2];
	
	b[0] = points[2]->getPoint()[0] - points[1]->getPoint()[0];
	b[1] = points[2]->getPoint()[1] - points[1]->getPoint()[1];
	b[2] = points[2]->getPoint()[2] - points[1]->getPoint()[2];

	normal[0] = a[1] * b[2] - a[2] * b[1];
	normal[1] = a[2] * b[0] - a[0] * b[2];
	normal[2] = a[0] * b[1] - a[1] * b[0];

	// make the normals face in the right direction
	if (normal[1] < 0) {
		normal[0] = -normal[0];
		normal[1] = -normal[1];
		normal[2] = -normal[2];
	}

	for (int i = 0; i < 3; i++) {
		Point *point = points[i];
		const float *pointNormal = point->getNormal();

		point->setNormal(pointNormal[0] + normal[0],
				 pointNormal[1] + normal[1],
				 pointNormal[2] + normal[2]);
	}
	
	float len = (float) sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);

	if (len == 0.0)
		len = 1.0;
	
	normal[0] /= len;
	normal[1] /= len;
	normal[2] /= len;
}

bool Triangle::livesWithinXZ(float x, float z, float *hitPos) {
	float linePoint[] = {x, -MOUNTAIN_BASE, z};
	float lineVector[] = {0.0, 1.0, 0.0};

	float planePoint[3];
	planePoint[0] = points[2]->getPoint()[0];
	planePoint[1] = points[2]->getPoint()[1];
	planePoint[2] = points[2]->getPoint()[2];

	planePoint[0] -= linePoint[0];
	planePoint[1] -= linePoint[1];
	planePoint[2] -= linePoint[2];

	float dot1 = normal[0] * planePoint[0] + normal[1] * planePoint[1] + normal[2] * planePoint[2];
	float dot2 = normal[0] * lineVector[0] + normal[1] * lineVector[1] + normal[2] * lineVector[2];
	float t = dot1 / dot2;

	lineVector[0] *= t;
	lineVector[1] *= t;
	lineVector[2] *= t;

	hitPos[0] = linePoint[0] + lineVector[0];
	hitPos[1] = linePoint[1] + lineVector[1];
	hitPos[2] = linePoint[2] + lineVector[2];

	float a[] = {points[0]->getPoint()[0], points[0]->getPoint()[1]};
	float b[] = {points[1]->getPoint()[0], points[1]->getPoint()[1]};
	float c[] = {points[2]->getPoint()[0], points[2]->getPoint()[1]};
	float p[] = {hitPos[0], hitPos[1]};

	float denom = (a[0] - c[0]) * (b[1] - c[1]) - (b[0] - c[0]) * (a[1] - c[1]);
	u = ((p[0] - c[0]) * (b[1] - c[1]) - (b[0] - c[0]) * (p[1] - c[1])) / denom;
	v = ((a[0] - c[0]) * (p[1] - c[1]) - (p[0] - c[0]) * (a[1] - c[1])) / denom;
	w = 1 - u - v;

	return u >= 0.0 && u <= 1.0 && v >= 0.0 && v <= 1.0 && u + v <= 1.0;
}

void Triangle::getInterpolatedNormalAtLastKnownPoint(float *intNormal) {
	const float *aNormal = points[0]->getNormal();
	const float *bNormal = points[1]->getNormal();
	const float *cNormal = points[2]->getNormal();

	intNormal[0] = u * aNormal[0] + v * bNormal[0] + w * cNormal[0];
	intNormal[1] = u * aNormal[1] + v * bNormal[1] + w * cNormal[1];
	intNormal[2] = u * aNormal[2] + v * bNormal[2] + w * cNormal[2];
	
	float len = (float) sqrt(intNormal[0] * intNormal[0] + intNormal[1] * intNormal[1] + intNormal[2] * intNormal[2]);

	if (len == 0.0)
		len = 1.0;
	
	intNormal[0] /= len;
	intNormal[1] /= len;
	intNormal[2] /= len;
}
