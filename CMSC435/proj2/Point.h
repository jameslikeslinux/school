#ifndef POINT_H
#define POINT_H

class Point {
	public:
		Point(float x, float y, float z);
		void draw() const;
		const float* getPoint() const;
		void setPoint(float x, float y, float z);
		bool isModified() const;
		bool operator==(const Point& point) const;
	
	private:
		void colorize();

		float point[3], color[3];
		bool modified;
};

#endif
