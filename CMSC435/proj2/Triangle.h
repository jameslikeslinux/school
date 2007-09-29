#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vector>
using namespace std;

#include "Point.h"

class Triangle {
	public:
		Triangle(Point *point1, Point *point2, Point *point3, vector<Point*> *allPoints);
		vector<Triangle*> subdivide() const;
		void draw() const;
	
	private:
		Point *points[3];		
		vector<Point*> *allPoints;
};

#endif
