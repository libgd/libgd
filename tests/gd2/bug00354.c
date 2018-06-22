/**
 * We're testing GD2 image files which report illegal chunk counts. These should
 * not cause integer overflows or other issues, but instead simply fail to be
 * loaded.
 *
 * See also <https://github.com/libgd/libgd/issues/354>.
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;

	fp = gdTestFileOpen2("gd2", "bug00354a.gd2");
	gdTestAssert(fp != NULL);
	im = gdImageCreateFromGd2(fp);
	gdTestAssert(im == NULL);
	fclose(fp);

	fp = gdTestFileOpen2("gd2", "bug00354b.gd2");
	gdTestAssert(fp != NULL);
	im = gdImageCreateFromGd2(fp);
	gdTestAssert(im == NULL);
	fclose(fp);

	return gdNumFailures();
}
