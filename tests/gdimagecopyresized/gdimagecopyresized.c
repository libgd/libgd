/**
 * add test case for gdImageCopyResized
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr src;
	gdImagePtr dst;
	gdImagePtr dst_tc;

	/* We prepare two image type: Palette and True Color */
	/* src image is white, while dst and dst_tc is black */
	src = gdImageCreate(64, 64);
	gdImageColorAllocate(src, 255, 255, 255);
	dst = gdImageCreate(128, 128);
	gdImageColorAllocate(dst, 0, 0, 0);
	dst_tc = gdImageCreateTrueColor(128, 128);

	/* Copy src to two image type separately */
	/* We copy src to the center of dst and dst_tc */
	gdImageCopyResized(dst, src, 32, 32, 0, 0, 64, 64, 64, 64);
	gdImageCopyResized(dst_tc, src, 32, 32, 0, 0, 64, 64, 64, 64);

	gdAssertImageEqualsToFile("gdimagecopyresized/gdimagecopyresized_exp.png", dst);
	gdAssertImageEqualsToFile("gdimagecopyresized/gdimagecopyresized_exp.png", dst_tc);

	gdImageDestroy(src);
	gdImageDestroy(dst);
	gdImageDestroy(dst_tc);

	return gdNumFailures();
}
