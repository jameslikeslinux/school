/*
 * Point.cpp
 * James Lee <jlee23@umbc.edu>
 *
 * Implementation for the Point class.
 * If I had time, I would be more consistent in my usage of either three
 * floats or arrays to represent position and color.  Oh well.
 */
#include <GL/gl.h>
#include <cmath>
#include "Point.h"
#include "Globals.h"

Point::Point(float x, float y, float z) {
	point[0] = x;
	point[1] = y;
	point[2] = z;

	color[0] = 1.0;
	color[1] = 1.0;
	color[2] = 1.0;

	normal[0] = 0;
	normal[1] = 0;
	normal[2] = 0;

	modified = false;

	colorize();
}

void Point::draw() const {
	glNormal3fv(normal);
	glColor3fv(color);
	glVertex3fv(point);
}

const float* Point::getPoint() const {
	return point;
}

void Point::setPoint(float x, float y, float z) {
	point[0] = x;
	point[1] = y;
	point[2] = z;
	modified = true;
	
	colorize();
}

const float* Point::getNormal() const {
	return normal;
}

void Point::setNormal(float x, float y, float z) {
	normal[0] = x;
	normal[1] = y;
	normal[2] = z;
}

void Point::normalize() {
	float len = (float) sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);

	if (len == 0.0)
		len = 1.0;
	
	normal[0] /= len;
	normal[1] /= len;
	normal[2] /= len;
}

bool Point::isModified() const {
	return modified;
}

bool Point::operator==(const Point& point) const {
	return this->point[0] == point.point[0] &&
	       this->point[1] == point.point[1] &&
	       this->point[2] == point.point[2];
}

void Point::colorize() {
	float height = point[1] + MOUNTAIN_BASE;
	
	if (height < 0.0) {
		color[0] = 0.3;
		color[1] = 0.3;
		color[2] = 0.3;
	} else if (height >= 0.0 && height < 25.0) {
		color[0] = 0.0;
		color[1] = 0.2;
		color[2] = 0.0;
	} else if (height >= 25.0 && height < 60.0) {
		color[0] = 0.3;
		color[1] = 0.3;
		color[2] = 0.3;
	} else if (height >= 60.0) {
		color[0] = 1.0;
		color[1] = 1.0;
		color[2] = 1.0;
	}
}
