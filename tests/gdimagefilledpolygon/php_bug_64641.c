/*
	Test drawing of 1-dimensional filled polygons

	We're drawing a vertical and a horizontal 1-dimensional filled polygon,
	which is supposed to result in a vertical and a horizontal line.

	See also <https://bugs.php.net/64641>.
*/

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	gdPointPtr points;

	im = gdImageCreateTrueColor(640, 480);

	points = (gdPointPtr)calloc(3, sizeof(gdPoint));
	gdTestAssert(points != NULL);

	/* vertical line */
	points[0].x = 100;
	points[0].y = 100;
	points[1].x = 100;
	points[1].y = 200;
	points[2].x = 100;
	points[2].y = 300;
	gdImageFilledPolygon(im, points, 3, 0xFFFF00);

	/* horizontal line */
	points[0].x = 300;
	points[0].y = 200;
	points[1].x = 400;
	points[1].y = 200;
	points[2].x = 500;
	points[2].y = 200;
	gdImageFilledPolygon(im, points, 3, 0xFFFF00);

	gdAssertImageEqualsToFile("gdimagefilledpolygon/php_bug_64641.png", im);

	free(points);
	gdImageDestroy(im);

	return gdNumFailures();
}
