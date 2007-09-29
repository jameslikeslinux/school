#include "Point.h"

Point::Point(float x, float y, float z) {
	point[0] = x;
	point[1] = y;
	point[2] = z;
}

const float* Point::getPoint() const {
	return point;
}

const float* Point::getColor() const {
	return color;
}
