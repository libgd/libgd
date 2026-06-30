#include "gd.h"
#include "gdtest.h"

/* Test for issue #547: gdImageDashedLine perpendicular width uses wrong trig
 *
 * gdImageDashedLine computes a perpendicular stroke width (wid) using
 *   wid = thick / sin(atan2(dy, dx))
 * for the horizontal-ish case, but should use cos() since the stroke is
 * perpendicular to the line direction.
 *
 * At 45 degrees sin == cos so the bug is invisible there.  This test uses
 * a shallow angle where the difference is large.  */
int main(void)
{
	gdImagePtr im;
	int white;
	int y, dash_pixel_count;

	im = gdImageCreate(65, 65);
	gdImageColorAllocate(im, 0, 0, 0); /* black background = index 0 */
	white = gdImageColorAllocate(im, 255, 255, 255);
	im->thick = 3;

	/* Shallow descending line: (0,32) -> (64,0)
	 *   dx=64, dy=32  →  angle ≈ 26.57°
	 *   sin ≈ 0.447  →  buggy wid = 3/0.447 ≈ 6  (too wide)
	 *   cos ≈ 0.894  →  correct wid = 3/0.894 ≈ 3
	 * With the bug the dash stroke covers 6 pixels instead of 3 at each
	 * step, causing extra pixels outside the expected band.  */
	gdImageDashedLine(im, 0, 32, 64, 0, white);

	/* Count pixels set at x=0 (the start of the line).
	 * With the correct cos-based width of 3 we expect exactly 3 pixels lit
	 * centered on y=32: (0,31), (0,32), (0,33).
	 * With the buggy sin-based width of 6 we get 6 pixels including (0,29)
	 * and (0,30).  */
	dash_pixel_count = 0;
	for (y = 28; y <= 36; y++) {
		if (gdImageGetPixel(im, 0, y) == white)
			dash_pixel_count++;
	}

	if (dash_pixel_count != 3) {
		gdTestErrorMsg("expected 3 vertical pixels at x=0, got %d "
			       "(sin/cos bug present — wid is wrong)\n",
			       dash_pixel_count);
		gdImageDestroy(im);
		return 1;
	}

	/* Verify the specific pixels that should be lit. */
	if (gdImageGetPixel(im, 0, 31) != white ||
	    gdImageGetPixel(im, 0, 32) != white ||
	    gdImageGetPixel(im, 0, 33) != white) {
		gdTestErrorMsg("expected white at (0,31)-(0,33), "
			       "but got %d,%d,%d\n",
			       gdImageGetPixel(im, 0, 31),
			       gdImageGetPixel(im, 0, 32),
			       gdImageGetPixel(im, 0, 33));
		gdImageDestroy(im);
		return 1;
	}

	/* With the bug, sin-based wid=6 would also light (0,29) and (0,30). */
	if (gdImageGetPixel(im, 0, 29) == white ||
	    gdImageGetPixel(im, 0, 30) == white) {
		gdTestErrorMsg("unexpected white at (0,29) or (0,30) — "
			       "perpendicular width too large (sin/cos bug)\n");
		gdImageDestroy(im);
		return 1;
	}

	gdImageDestroy(im);
	return 0;
}
