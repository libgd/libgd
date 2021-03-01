/**
 * Test that failure to convert to AVIF returns NULL
 *
 * We are creating an image, set its width to zero, and pass this image to
 * `gdImageAvifPtr()` which is supposed to fail, and as such should return NULL.
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

	src->sx = 0;

	dst = gdImageHeifPtrEx(src, &size, 100, GD_HEIF_CODEC_AV1, GD_HEIF_CHROMA_444);
	gdTestAssert(dst == NULL);

	gdImageDestroy(src);

	return gdNumFailures();
}
