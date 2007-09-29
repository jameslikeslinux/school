#include <iostream>
#include "Triangle.h"

Point* averagePoint(const Point *point1, const Point *point2) {
	float x = (point1->getPoint()[0] + point2->getPoint()[0]) / 2;
	float y = (point1->getPoint()[1] + point2->getPoint()[1]) / 2;
	float z = (point1->getPoint()[2] + point2->getPoint()[2]) / 2;

	return new Point(x, y, z);
}

Point* getPoint(vector<Point*> *allPoints, Point *point) {
	for (unsigned int i = 0; i < allPoints->size(); i++) {
		if (*allPoints->at(i) == *point) {
			delete point;
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

void Triangle::draw() const {
	cout << "\t" << points[0]->getPoint()[0] << " " << points[0]->getPoint()[1] << " " << points[0]->getPoint()[2] << endl;
	cout << "\t" << points[1]->getPoint()[0] << " " << points[1]->getPoint()[1] << " " << points[1]->getPoint()[2] << endl;
	cout << "\t" << points[2]->getPoint()[0] << " " << points[2]->getPoint()[1] << " " << points[2]->getPoint()[2] << endl;
}
