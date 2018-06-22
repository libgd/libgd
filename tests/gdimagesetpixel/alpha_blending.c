/**
 * Testing all effects
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int effects[] = {
		gdEffectReplace, gdEffectAlphaBlend, gdEffectNormal,
		gdEffectOverlay, gdEffectMultiply
	};
	int red[3], blue[3];
	int x, y, i, j;
	char *path;

	red[0] = gdTrueColorAlpha(0, 0, 255, 127);
	red[1] = gdTrueColorAlpha(0, 0, 255, 63);
	red[2] = gdTrueColorAlpha(0, 0, 255, 0);
	blue[0] = gdTrueColorAlpha(255, 0, 0, 127);
	blue[1] = gdTrueColorAlpha(255, 0, 0, 63);
	blue[2] = gdTrueColorAlpha(255, 0, 0, 0);

	im = gdImageCreateTrueColor(120, 180);

	gdImageAlphaBlending(im, gdEffectReplace);
	for (j = 0; j < 3; j++) {
		y = 60 * j;
		gdImageFilledRectangle(im, 0,y, 119,y+59, red[j]);
	}

	for (i = 0; i < 5; i++) {
		x = 20 * i;
		gdImageAlphaBlending(im, effects[i]);
		for (j = 0; j < 9; j++) {
			y = 20 * j;
			gdImageFilledRectangle(im, x+20,y, x+39,y+19, blue[j % 3]);
		}
	}

	gdImageSaveAlpha(im, 1);

	path = gdTestFilePath2("gdimagesetpixel", "alphablending_exp.png");
	gdAssertImageEqualsToFile(path, im);
	gdFree(path);

	gdImageDestroy(im);

	return gdNumFailures();
}
