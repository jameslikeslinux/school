#include <ri.h>
#include "draw.h"

int main() {
	RtPoint floor[4] = {{-1000, 0, -1000}, {-1000, 0, 1000}, {1000, 0, 1000}, {1000, 0, -1000}};
	RtPoint lfLegFrom = {-29.0, 0.0, -14.0}, lfLegTo = {-28.0, 29.0, -13.0};
	RtPoint lbLegFrom = {-29.0, 0.0, 14.0}, lbLegTo = {-28.0, 29.0, 13.0};
	RtPoint rfLegFrom = {29.0, 0.0, -14.0}, rfLegTo = {28, 29.0, -13.0};
	RtPoint rbLegFrom = {29.0, 0.0, 14.0}, rbLegTo = {28, 29.0, 13.0};
	RtPoint tableTopFrom = {-30.0, 29.0, -15.0}, tableTopTo = {30.0, 30.0, 15.0};
	RtColor brown = {0.5, 0.4, 0.3};
	RtColor black = {0.1, 0.1, 0.1};

	RtPoint spotlightFrom = {-10.0, 45.0, 12.0};
	RtPoint spotlightTo = {-9.0, 44.0, 11.5};
	RtFloat pointlightIntensity = 50, spotlightIntensity = 500, ambientlightIntensity = 0.3;

	RiBegin("proj1.rib");
		RiDisplay("proj1.tif", "file", "rgba", RI_NULL);
		RiFormat(800, 600, 1);

		RiProjection("perspective", RI_NULL);

		RiRotate(30.0, -1.0, 1.0, 0.0);
		RiTranslate(-24.0, -48.0, 36.0);

		RiLightSource("pointlight", "intensity", &pointlightIntensity, "from", spotlightFrom, RI_NULL);
		RiLightSource("spotlight", "intensity", &spotlightIntensity, "from", spotlightFrom, "to", spotlightTo, RI_NULL);
		RiLightSource("ambientlight", "intensity", &ambientlightIntensity, RI_NULL);

		RiWorldBegin();
			RiPolygon(4, RI_P, floor, RI_NULL);

			RiColor(brown);
			RiSurface("matte", RI_NULL);
			drawCube(lfLegFrom, lfLegTo);
			drawCube(lbLegFrom, lbLegTo);
			drawCube(rfLegFrom, rfLegTo);
			drawCube(rbLegFrom, rbLegTo);
			drawCube(tableTopFrom, tableTopTo);

			RiTransformBegin();
				RiTranslate(-12.0, 48.0, 12.0);
				RiRotate(100, 1.0, 0, 0);
				RiRotate(45, 0, 1.0, 0);

				RiColor(black);
				RiSurface("plastic", RI_NULL);
				RiParaboloid(4, 0, 6, 360, RI_NULL);
			RiTransformEnd();
		RiWorldEnd();
	RiEnd();

	return 0;
}
