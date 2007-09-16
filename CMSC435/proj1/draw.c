#include "draw.h"

void drawCube(RtPoint from, RtPoint to) {
	RtPoint front[4] = {{from[0], from[1], from[2]}, {from[0], to[1], from[2]}, {to[0], to[1], from[2]}, {to[0], from[1], from[2]}};
	RtPoint back[4] = {{from[0], from[1], to[2]}, {from[0], to[1], to[2]}, {to[0], to[1], from[2]}, {to[0], from[1], to[2]}};
	RtPoint left[4] = {{from[0], from[1], from[2]}, {from[0], to[1], from[2]}, {from[0], to[1], to[2]}, {from[0], from[1], to[2]}};
	RtPoint right[4] = {{to[0], from[1], from[2]}, {to[0], to[1], from[2]}, {to[0], to[1], to[2]}, {to[0], from[1], to[2]}};
	RtPoint top[4] = {{from[0], to[1], from[2]}, {from[0], to[1], to[2]}, {to[0], to[1], to[2]}, {to[0], to[1], from[2]}};
	RtPoint bottom[4] = {{from[0], from[1], from[2]}, {from[0], from[1], to[2]}, {to[0], from[1], to[2]}, {to[0], from[1], from[2]}};

	RiPolygon(4, RI_P, (RtPointer) front, RI_NULL);
	RiPolygon(4, RI_P, (RtPointer) back, RI_NULL);
	RiPolygon(4, RI_P, (RtPointer) left, RI_NULL);
	RiPolygon(4, RI_P, (RtPointer) right, RI_NULL);
	RiPolygon(4, RI_P, (RtPointer) top, RI_NULL);
	RiPolygon(4, RI_P, (RtPointer) bottom, RI_NULL);
}
