#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vector>
using namespace std;

#include "Point.h"

class Triangle {
	public:
		Triangle(const Point* point1, const Point* point2, const Point* point3);
		vector<Triangle> subdivide() const;
		void draw() const;
	
	private:
		const Point* points[3];		
};

#endif
