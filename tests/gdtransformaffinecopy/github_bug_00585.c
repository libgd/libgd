/*
 * Test that gdTransformAffineCopy() does not change the interpolation method
 *
 * See <https://github.com/libgd/libgd/issues/585>
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr src, dst;
	gdRect rect = {0, 0, 8, 8};
	double matrix[] = {1, 1, 1, 1, 1, 1};
	int res;
	gdInterpolationMethod old_m, new_m;

	src = gdImageCreateTrueColor(8, 8);
	gdTestAssert(src != NULL);
	dst = gdImageCreateTrueColor(8, 8);
	gdTestAssert(dst != NULL);

	res = gdImageSetInterpolationMethod(src, GD_CATMULLROM);
	gdTestAssert(res == GD_TRUE);
	old_m = gdImageGetInterpolationMethod(src);
	gdTransformAffineCopy(dst, 0, 0, src, &rect, matrix);
	new_m = gdImageGetInterpolationMethod(src);
	gdTestAssert(new_m == old_m);

	gdImageDestroy(src);
	gdImageDestroy(dst);
	return gdNumFailures();
}
