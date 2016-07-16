/* See <https://bugs.php.net/bug.php?id=43828>. */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int transparent, color;

	im = gdImageCreateTrueColor(100, 100);
	transparent = gdImageColorAllocateAlpha(im, 255, 255, 255, 80);
	gdImageFilledRectangle(im, 0,0, 99,99, transparent);
	color = gdImageColorAllocateAlpha(im, 0, 255, 0, 100);
	gdImageFilledArc(im, 49,49, 99,99, 0,360, color, gdPie);

	gdAssertImageEqualsToFile("gdimagefilledarc/php_bug43828_exp.png", im);

	gdImageDestroy(im);
	return gdNumFailures();
}
