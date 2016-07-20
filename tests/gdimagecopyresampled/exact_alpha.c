/* Test for exact colors of gdImageCopyResampled */

#include "gd.h"
#include "gdtest.h"


#define EXP_RED    66
#define EXP_GREEN  66
#define EXP_BLUE  133
#define EXP_ALPHA  32


int main()
{
	gdImagePtr im, copy;
	int solid, transparent, color;
	int red, green, blue, alpha;
	int i, j;

	/* create the source image */
	im = gdImageCreateTrueColor(10, 10);
	gdImageAlphaBlending(im, 0);
	solid = gdImageColorAllocate(im, 0, 100, 150);
	transparent = gdImageColorAllocateAlpha(im, 200, 0, 100, 64);

	/* draw a checker pattern */
	for (i = 0; i < gdImageSX(im); i++) {
		for (j = 0; j < gdImageSY(im); j++) {
			gdImageSetPixel(im, i, j, (i%2 != j%2 ? solid : transparent));
		}
	}

	/* create the destination image */
	copy = gdImageCreateTrueColor(5, 5);
	gdImageAlphaBlending(copy, 0);
	gdImageSaveAlpha(copy, 1);
	gdImageCopyResampled(copy, im, 0,0, 0,0, 5,5, 10, 10);

	/* assert all pixels have the same color */
	color = gdImageTrueColorPixel(copy, 3, 3);
	for (i = 0; i < gdImageSX(copy); i++) {
		for (j = 0; j < gdImageSY(copy); j++) {
			gdTestAssert(gdImageTrueColorPixel(copy, i, j) == color);
		}
	}

	/* assign actual component values */
	red = gdTrueColorGetRed(color);
	green = gdTrueColorGetGreen(color);
	blue = gdTrueColorGetBlue(color);
	alpha = gdTrueColorGetAlpha(color);

	/* test for expected component values */
	gdTestAssertMsg(red >= EXP_RED - 1 && red <= EXP_RED + 1,
					"red: expected roughly %d, got %d\n", EXP_RED, red);
	gdTestAssertMsg(green >= EXP_GREEN - 1 && green <= EXP_GREEN + 1,
					"green: expected roughly %d, got %d\n", EXP_GREEN, green);
	gdTestAssertMsg(blue >= EXP_BLUE - 1 && blue <= EXP_BLUE + 1,
					"blue: expected roughly %d, got %d\n", EXP_BLUE, blue);
	gdTestAssertMsg(alpha >= EXP_ALPHA - 1 && alpha <= EXP_ALPHA + 1,
					"alpha: expected roughly %d, got %d\n", EXP_ALPHA, alpha);

	gdImageDestroy(copy);
	gdImageDestroy(im);

	return gdNumFailures();
}
