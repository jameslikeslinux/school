/*
 * Point.h
 * James Lee <jlee23@umbc.ed>
 *
 * Class definition for a point which is essentially a vertex position and
 * color.
 */
#ifndef POINT_H
#define POINT_H

class Point {
	public:
		Point(float x, float y, float z);

		// This expects to be called only from the Triangle's draw method.
		// Don't call it from elsewhere!
		void draw() const;

		const float* getPoint() const;
		void setPoint(float x, float y, float z);

		const float* getNormal() const;
		void setNormal(float x, float y, float z);
		void normalize();

		bool isModified() const;
		bool operator==(const Point& point) const;
	
	private:
		void colorize();

		float point[3], color[3], normal[3];
		bool modified;
};

#endif
