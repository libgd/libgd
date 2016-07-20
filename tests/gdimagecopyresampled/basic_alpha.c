/* Testing basic gdImageCopyResampled() functionality with alpha channel */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im, copy;
	int white, yellow, magenta, cyan;

	im = gdImageCreateTrueColor(400, 300);
	gdImageAlphaBlending(im, 0);

	/* allocate some alpha colors */
	white = gdImageColorAllocateAlpha(im, 255, 255, 255, 31);
	yellow = gdImageColorAllocateAlpha(im, 128, 0, 128, 63);
	magenta = gdImageColorAllocateAlpha(im, 128, 128, 0, 95);
	cyan = gdImageColorAllocateAlpha(im, 0, 128, 128, 127);

	/* draw background */
	gdImageFilledRectangle(im, 0,0, 399,299, white);

	/* draw some basic shapes */
	gdImageFilledEllipse(im, 100,100, 150,100, yellow);
	gdImageSetThickness(im, 5);
	gdImageLine(im, 50,250, 350,50, magenta);
	gdImageRectangle(im, 200,100, 350,250, cyan);

	/* create a diminished, resampled copy of the original image */
	copy = gdImageCreateTrueColor(200, 200);
	gdImageAlphaBlending(copy, 0);
	gdImageSaveAlpha(copy, 1);
	gdImageCopyResampled(copy, im, 0,0, 0,0, 200,200, 400,300);
	gdImageDestroy(im);

	gdAssertImageEqualsToFile("gdimagecopyresampled/basic_alpha_exp.png", copy);

	gdImageDestroy(copy);
	return gdNumFailures();
}
