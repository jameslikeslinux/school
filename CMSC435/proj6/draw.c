/*
 * draw.c
 * James Lee <jlee23@umbc.edu>
 *
 * Contains functions that do the meat of the drawing.
 */

#include "draw.h"

/* Draw a rectangular prism which spans two given opposite corners */
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

/* Calculates the corners of cubes to draw */
void drawTable(RtFloat length, RtFloat width, RtFloat height) {
	if (length < 4 || width < 4 || height < 4)
		return;

	/* The legs are 1" wide and sit 1" in from the edge of the top */
	RtPoint lfLegFrom = {-(length / 2 - 1), 0.0, -(width / 2 - 1)}, lfLegTo = {-(length / 2 - 2), (height - 1), -(width / 2 - 2)};
	RtPoint lbLegFrom = {-(length / 2 - 1), 0.0, (width / 2 - 1)}, lbLegTo = {-(length / 2 - 2), (height - 1), (width / 2 - 2)};
	RtPoint rfLegFrom = {(length / 2 - 1), 0.0, -(width / 2 - 1)}, rfLegTo = {(length / 2 - 2), (height - 1), -(width / 2 - 2)};
	RtPoint rbLegFrom = {(length / 2 - 1), 0.0, (width / 2 - 1)}, rbLegTo = {(length / 2 - 2), (height - 1), (width / 2 - 2)};
	RtPoint tableTopFrom = {-(length / 2), (height - 1), -(width / 2)}, tableTopTo = {(length / 2), height, (width / 2)};
	
	drawCube(lfLegFrom, lfLegTo);
	drawCube(lbLegFrom, lbLegTo);
	drawCube(rfLegFrom, rfLegTo);
	drawCube(rbLegFrom, rbLegTo);

	RiSurface("tile", RI_NULL);
	drawCube(tableTopFrom, tableTopTo);
}

void drawLamp(RtPoint position, RtFloat height) {
	/* Draw the base of the lamp */
	RiTransformBegin();
		RiTranslate(position[0], position[1] + 1, position[2]);
		RiRotate(90.0, 1.0, 0.0, 0.0);
		RiDisk(0.0, 4.0, 360.0, RI_NULL);
		RiCylinder(4.0, 0.0, 1.0, 360.0, RI_NULL);
	RiTransformEnd();

	/* Draw the stem of the lamp */
	RiTransformBegin();
		RiTranslate(position[0], position[1] + height + 1, position[2]);
		RiRotate(90.0, 1.0, 0.0, 0.0);
		RiCylinder(0.5, 0.0, height, 360.0, RI_NULL);
	RiTransformEnd();

	/* Draw the bell shaped part of the lamp */
	RiTransformBegin();
		RiTranslate(position[0], position[1] + height + 3, position[2]);
		RiRotate(100, 1.0, 0, 0);
		RiRotate(45, 0, 1.0, 0);
		RiParaboloid(4, 0, 6, 360, RI_NULL);
	RiTransformEnd();
}

void drawPencil() {
	RtColor pink = {1.0, 0.5, 0.5};
	RtColor silver = {0.7, 0.7, 0.7};
	RtColor yellow = {1.0, 0.7, 0.0};
	RtColor tan = {0.9, 0.8, 0.5};
	RtColor black = {0.1, 0.1, 0.1};

	RtPoint pencilSide[4] = {{-4.0, 0.1, -0.1}, {-4.0, 0.1, 0.1}, {4.0, 0.1, 0.1}, {4.0, 0.1, -0.1}};

	RiColor(yellow);
	RiSurface("paintedmatte", RI_NULL);

	/* Draw all six sides of a pencil, each 8.0"x0.2" in size */
	RiPolygon(4, RI_P, pencilSide, RI_NULL);
	RiRotate(60.0, 1.0, 0.0, 0.0);
	RiPolygon(4, RI_P, pencilSide, RI_NULL);
	RiRotate(60.0, 1.0, 0.0, 0.0);
	RiPolygon(4, RI_P, pencilSide, RI_NULL);
	RiRotate(60.0, 1.0, 0.0, 0.0);
	RiPolygon(4, RI_P, pencilSide, RI_NULL);
	RiRotate(60.0, 1.0, 0.0, 0.0);
	RiPolygon(4, RI_P, pencilSide, RI_NULL);
	RiRotate(60.0, 1.0, 0.0, 0.0);
	RiPolygon(4, RI_P, pencilSide, RI_NULL);
	
	/* Draw the eraser */
	RiTransformBegin();
		RiTranslate(4.2, 0.0, 0.0);

		/* Draw the metal part around the eraser */
		RiTransformBegin();
			RiTranslate(-0.2, 0.0, 0.0);
			RiRotate(-90.0, 0.0, 1.0, 0.0);

			RiColor(silver);
			RiSurface("metal", RI_NULL);
			RiCylinder(0.15, 0.0, 0.3, 360.0, RI_NULL);
		RiTransformEnd();

		RiRotate(-90.0, 0.0, 1.0, 0.0);

		RiColor(pink);
		RiSurface("matte", RI_NULL);

		RiDisk(0.0, 0.15, 360, RI_NULL);
		RiCylinder(0.15, 0.0, 0.2, 360, RI_NULL);
	RiTransformEnd();

	RiColor(tan);
	RiSurface("matte", RI_NULL);

	/* Draw the exposed wood part at the bottom of the pencil */
	RiTranslate(-4.0, 0.0, 0.0);
	RiTransformBegin();
		RiRotate(-90.0, 0.0, 1.0, 0.0);
		RiCone(1.0, 0.15, 360, RI_NULL);
	RiTransformEnd();

	/* Draw the lead of the pencil */
	RiTranslate(-0.7, 0.0, 0.0);
	RiRotate(-90.0, 0.0, 1.0, 0.0);

	RiColor(black);
	RiCone(0.3, 0.05, 360, RI_NULL);
}
