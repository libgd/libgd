#include "gd.h"
#include "gdtest.h"
#include <math.h>

int main() {
	gdImagePtr im;
	int white, black;
	int cx = 100, cy = 100, w = 150, h = 150;
	int thick = 11;
	int error = 0;

	im = gdImageCreate(cx * 2, cy * 2);
	gdTestAssert(im != NULL);
	white = gdImageColorAllocate(im, 255, 255, 255);
	black = gdImageColorAllocate(im, 0, 0, 0);

	gdImageSetThickness(im, thick);
	gdImageArc(im, cx, cy, w, h, 0, 360, black);

	/* Verify there are no gaps at the 45-degree transition points.
	 * The arc has radius 75 (w/2). With thickness 11, the band covers
	 * roughly radius 70..80. Check the band center at all four
	 * diagonal angles. */
	int angles[] = {45, 135, 225, 315};
	for (int a = 0; a < 4; a++) {
		double rad = angles[a] * 3.14159265 / 180.0;
		int r = 75;
		int px = cx + (int)(r * cos(rad));
		int py = cy - (int)(r * sin(rad));
		if (gdImageGetPixel(im, px, py) == white) {
			gdTestErrorMsg("gap at angle=%d (%d,%d)\n", angles[a], px, py);
			error = 1;
		}
	}

	gdImageDestroy(im);
	return error;
}
