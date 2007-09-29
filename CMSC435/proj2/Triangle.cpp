#include "Triangle.h"

Triangle::Triangle(const Point* point1, const Point* point2, const Point* point3) {
	points[0] = point1;
	points[1] = point2;
	points[2] = point3;
}

vector<Triangle> Triangle::subdivide() const {
	return vector<Triangle>();
}

void Triangle::draw() const {

}
