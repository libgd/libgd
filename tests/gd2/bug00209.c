/**
 * Regression test for github issue #209
 *
 * We're testing that bug00209.gd2, which claims to have 12336 x 48 pixels, but
 * actually provides not enough image data, is rejected, i.e. that
 * gdImageCreateFromGd2() returns NULL
 *
 * See <https://github.com/libgd/libgd/issues/209>.
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;

	fp = gdTestFileOpen2("gd2", "bug00209.gd2");
	gdTestAssert(fp != NULL);
	im = gdImageCreateFromGd2(fp);
	gdTestAssert(im == NULL);
	fclose(fp);

	return gdNumFailures();
}
