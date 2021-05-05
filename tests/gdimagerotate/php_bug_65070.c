/**
 * Regression test for <https://github.com/libgd/libgd/issues/319>
 *
 * We're testing that the rotated image actually has the requested background
 * color.
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr src, dst;
	FILE *fp;
	int black;
	char *path;

	fp = gdTestFileOpen2("gdimagerotate", "php_bug_65070.gif");
	src = gdImageCreateFromGif(fp);
	fclose(fp);

	black = gdImageColorAllocateAlpha(src, 0, 0, 0, 0);
	dst = gdImageRotateInterpolated(src, 30.0, black);

	path = gdTestFilePath2("gdimagerotate", "php_bug_65070_exp.png");
	gdAssertImageEqualsToFile(path, dst);
	gdFree(path);

	gdImageDestroy(src);
	gdImageDestroy(dst);

	return gdNumFailures();
}
