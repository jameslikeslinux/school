#ifndef POINT_H
#define POINT_H

class Point {
	public:
		Point(float x, float y, float z);
		const float* getPoint() const;
		const float* getColor() const;
	
	private:
		float point[3], color[3];
};

#endif
