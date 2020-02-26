#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gdtest.h"

#define PI 3.141592
#define DELTA (PI/8)

static int EXPECT[16][8] = {
	{500, 400, 628, 400, 628, 376, 500, 376},
	{492, 362, 611, 312, 601, 290, 483, 339},
	{470, 330, 561, 239, 544, 221, 453, 312},
	{437, 308, 486, 189, 464, 180, 414, 299},
	{400, 301, 400, 173, 376, 173, 376, 301},
	{363, 309, 313, 190, 291, 200, 340, 318},
	{332, 331, 241, 240, 223, 257, 314, 348},
	{311, 363, 192, 314, 183, 336, 302, 386},
	{304, 399, 176, 399, 176, 423, 304, 423},
	{312, 435, 193, 485, 203, 507, 321, 458},
	{333, 465, 242, 556, 259, 574, 350, 483},
	{364, 486, 315, 605, 337, 614, 387, 495},
	{399, 492, 399, 620, 423, 620, 423, 492},
	{434, 484, 484, 603, 506, 593, 457, 475},
	{463, 464, 554, 555, 572, 538, 481, 447},
	{483, 434, 602, 483, 611, 461, 492, 411},
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
