#include "Point.h"

Point::Point(float x, float y, float z) {
	point[0] = x;
	point[1] = y;
	point[2] = z;
	modified = false;
}

const float* Point::getPoint() const {
	return point;
}

void Point::setPoint(float x, float y, float z) {
	point[0] = x;
	point[1] = y;
	point[2] = z;
	modified = true;
}

bool Point::isModified() const {
	return modified;
}

bool Point::operator==(const Point& point) const {
	return this->point[0] == point.point[0] &&
	       this->point[1] == point.point[1] &&
	       this->point[2] == point.point[2];
}
