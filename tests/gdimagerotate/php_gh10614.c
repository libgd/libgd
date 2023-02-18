/**
 * Regression test for <https://github.com/php/php-src/issues/10614>
 *
 * We're testing that the alpha is preserved upon rotating for square angles.
 */

#include "gd.h"
#include "gdtest.h"
#include "string.h"

int main()
{
	gdImagePtr src, dst;
	FILE *fp;
	char *path, buf[128];
	const int squareAngles[] = { 0, 90, 180, 270 };
	int i;

	fp = gdTestFileOpen2("gdimagerotate", "php_gh10614.png");
	src = gdImageCreateFromPng(fp);
	fclose(fp);

	for (i = 0; i < sizeof(squareAngles) / sizeof(squareAngles[0]); i++) {
		snprintf(buf, sizeof(buf) - 1, "php_gh10614_%03d_exp.png", squareAngles[i]);
		path = gdTestFilePath2("gdimagerotate", buf);
		dst = gdImageRotateInterpolated(src, squareAngles[i], 0);
		gdAssertImageEqualsToFile(path, dst);
		gdImageDestroy(dst);
		gdFree(path);
	}

	gdImageDestroy(src);

	return gdNumFailures();
}
