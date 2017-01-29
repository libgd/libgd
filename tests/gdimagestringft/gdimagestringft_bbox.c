#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gdtest.h"

#define PI 3.141592
#define DELTA (PI/8)

static int EXPECT[16][8] = {
	{498, 401, 630, 401, 630, 374, 498, 374},
	{491, 364, 613, 313, 602, 288, 481, 338},
	{470, 332, 563, 239, 544, 219, 451, 312},
	{438, 310, 488, 189, 463, 178, 412, 300},
	{401, 303, 401, 171, 374, 171, 374, 303},
	{365, 310, 314, 188, 289, 199, 339, 320},
	{334, 331, 241, 238, 221, 257, 314, 350},
	{313, 362, 192, 312, 181, 337, 303, 388},
	{306, 398, 174, 398, 174, 425, 306, 425},
	{313, 433, 191, 484, 202, 509, 323, 459},
	{333, 463, 240, 556, 259, 576, 352, 483},
	{363, 484, 313, 605, 338, 616, 389, 494},
	{398, 490, 398, 622, 425, 622, 425, 490},
	{432, 483, 483, 605, 508, 594, 458, 473},
	{461, 464, 554, 557, 574, 538, 481, 445},
	{481, 435, 602, 485, 613, 460, 491, 409},
};

int main()
{
	char *path;
	gdImagePtr im;
	int black;
	double cos_t, sin_t;
	int x, y, temp;
	int i, j;
	int brect[8];
	int error = 0;
	FILE *fp;

	/* disable subpixel hinting */
	putenv("FREETYPE_PROPERTIES=truetype:interpreter-version=35");

	path = gdTestFilePath("freetype/DejaVuSans.ttf");
	im = gdImageCreate(800, 800);
	gdImageColorAllocate(im, 0xFF, 0xFF, 0xFF); /* allocate white for background color */
	black = gdImageColorAllocate(im, 0, 0, 0);
	cos_t = cos(DELTA);
	sin_t = sin(DELTA);
	x = 100;
	y = 0;
	for (i = 0; i < 16; i++) {
		if (gdImageStringFT(im, brect, black, path, 24, DELTA*i, 400+x, 400+y, "ABCDEF")) {
			error = 1;
			goto done;
		}
		for (j = 0; j < 8; j++) {
			if (brect[j] != EXPECT[i][j]) {
				gdTestErrorMsg("(%d, %d) (%d, %d) (%d, %d) (%d, %d) expected, but (%d, %d) (%d, %d) (%d, %d) (%d, %d)\n",
				       EXPECT[i][0], EXPECT[i][1], EXPECT[i][2], EXPECT[i][3],
				       EXPECT[i][4], EXPECT[i][5], EXPECT[i][6], EXPECT[i][7],
				       brect[0], brect[1], brect[2], brect[3],
				       brect[4], brect[5], brect[6], brect[7]);
				error = 1;
				goto done;
			}
		}
		gdImagePolygon(im, (gdPointPtr)brect, 4, black);
		gdImageFilledEllipse(im, brect[0], brect[1], 8, 8, black);
		temp = (int)(cos_t * x + sin_t * y);
		y = (int)(cos_t * y - sin_t * x);
		x = temp;
	}
	fp = gdTestTempFp();
	gdImagePng(im, fp);
	fclose(fp);
done:
	gdImageDestroy(im);
	gdFontCacheShutdown();
	free(path);
	return error;
}
