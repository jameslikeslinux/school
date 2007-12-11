/*
 * proj1.c
 * James Lee <jlee23@umbc.edu>
 *
 * This file contains the main function which sets up the scene by making the
 * standard calls to the RI to make a tiff with an oblique viewing angle.
 * It makes calls to other functions to do the more intensive drawing.
 *
 * The scene includes a 60"x30"x30" table, a lamp which produces light using a
 * pointlight and a spotlight, and a pencil that rests on a piece of paper.
 *
 * 1 unit = 1 inch
 */

#include "draw.h"

#define LAMPPOSX (-12.0)
#define LAMPPOSY 30.0
#define LAMPPOSZ 2.0
#define LAMPHEIGHT 15

int main() {
	RtFloat fov = 90;

	RtColor brown = {0.5, 0.4, 0.3};
	RtColor black = {0.1, 0.1, 0.1};
	RtColor white = {1.0, 1.0, 1.0};

	RtPoint lampPos = {LAMPPOSX, LAMPPOSY, LAMPPOSZ};
	RtPoint spotlightFrom = {LAMPPOSX + 2, LAMPPOSY + LAMPHEIGHT, LAMPPOSZ};
	RtPoint spotlightTo = {LAMPPOSX + 3, LAMPPOSY + LAMPHEIGHT - 1, LAMPPOSZ - 0.25};
	RtFloat pointlightIntensity = 50, spotlightIntensity = 500, ambientlightIntensity = 0.3;

	RtPoint paper[4] = {{-4.25, 0.0, -5.5}, {-4.25, 0.0, 5.5}, {4.25, 0.0, 5.5}, {4.25, 0.0, -5.5}};

	RiBegin("proj6.rib");
		RiDisplay("proj6.tif", "file", "rgba", RI_NULL);

		/* I like to see the details, so make a nice large image */
		RiFormat(1920, 1080, 1);

		/* Make the field of view larger for the larger image */
		RiProjection("perspective", "fov", &fov, RI_NULL);

		/* Act like a four foot tall person looking slightly down and to the left
		 * at the table */
		RiRotate(30, -1.0, 1.0, 0.0);
		RiTranslate(-12.0, -48.0, 18.0);

		/* These lights will look like they are coming from the lamp which is drawn
		 * later */
		RiLightSource("pointlight", "intensity", &pointlightIntensity, "from", spotlightFrom, RI_NULL);
		RiLightSource("spotlight", "intensity", &spotlightIntensity, "from", spotlightFrom, "to", spotlightTo, RI_NULL);

		/* This is barely on just to make things look nicer */
		RiLightSource("ambientlight", "intensity", &ambientlightIntensity, RI_NULL);

		RiWorldBegin();
			RiColor(brown);
			RiSurface("matte", RI_NULL);
			drawTable(60, 30, 30);

			RiColor(black);
			RiSurface("plastic", RI_NULL);
			drawLamp(lampPos, LAMPHEIGHT);

			RiTransformBegin();
				/* The pencil rests at an angle on top of the paper which will be drawn
				 * next */
				RiTranslate(0.0, 30.2, 0.0);
				RiRotate(-45.0, 0.0, 1.0, 0.0);
				drawPencil();
			RiTransformEnd();

			RiTransformBegin();
				/* The paper goes slightly above the table */
				RiTranslate(0.0, 30.01, 0.0);
				RiColor(white);
				RiPolygon(4, RI_P, paper, RI_NULL);
			RiTransformEnd();
		RiWorldEnd();
	RiEnd();

	return 0;
}
