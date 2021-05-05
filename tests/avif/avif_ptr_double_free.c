/**
 * Test that failure to convert to AVIF returns NULL
 *
 * We are creating an image, set its width to zero, and pass this image to
 * gdImageAvifPtr().
 * This is supposed to fail, and as such should return NULL.
 *
 * See also <https://github.com/libgd/libgd/issues/381>
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr src, dst;
	int size;

	src = gdImageCreateTrueColor(1, 10);
	gdTestAssert(src != NULL);

	src->sx = 0;  // making the width 0 should cause gdImageAvifPtr() to fail

	dst = gdImageAvifPtr(src, &size);
	gdTestAssert(dst == NULL);

	if (src)
		gdImageDestroy(src);

	if (dst)
		gdImageDestroy(dst);

	return gdNumFailures();
}
