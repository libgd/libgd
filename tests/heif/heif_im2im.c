/**
 * Check if it's any difference between the original bitmap and a encoded and
 * decoded `4:4:4` HEIF lossless image.
 */


#include "gd.h"
#include "gdtest.h"

#include <libheif/heif.h>

int main()
{
	gdImagePtr src = NULL, dst = NULL;
	int r, g, b;
	void *p = NULL;
	int size = 0;
	CuTestImageResult result = {0, 0};

	if (!gdTestAssertMsg(heif_get_version_number_major() == 1 && heif_get_version_number_minor() >= 9, "changing chroma subsampling is not supported in this libheif version\n"))
		return 77;

	if (!gdTestAssertMsg(heif_have_decoder_for_format(heif_compression_HEVC) && heif_have_encoder_for_format(heif_compression_HEVC), "HEVC codec support missing from libheif\n"))
		return 77;

	src = gdImageCreateTrueColor(100, 100);
	gdTestAssertMsg(src != NULL, "could not create src\n");
	/* libheif seems to have some rounding issues */
	r = gdImageColorAllocate(src, 0xFE, 0, 0);
	g = gdImageColorAllocate(src, 0, 0xFE, 0);
	b = gdImageColorAllocate(src, 0, 0, 0xFE);
	gdImageFilledRectangle(src, 0, 0, 99, 99, r);
	gdImageRectangle(src, 20, 20, 79, 79, g);
	gdImageEllipse(src, 70, 25, 30, 20, b);

	p = gdImageHeifPtrEx(src, &size, 200, GD_HEIF_CODEC_HEVC, GD_HEIF_CHROMA_444);
	gdTestAssertMsg(p != NULL, "return value of gdImageHeifPtrEx() is null\n");
	gdTestAssertMsg(size > 0, "gdImageHeifPtrEx() output size is non-positive\n");

	dst = gdImageCreateFromHeifPtr(size, p);
	gdTestAssertMsg(dst != NULL, "return value of gdImageCreateFromHeifPtr() is null\n");

	if (gdTestAssertMsg(src != NULL && dst != NULL, "cannot compare with NULL buffer"))
		gdTestImageDiff(src, dst, NULL, &result);
	else
		result.pixels_changed = 0;
	gdTestAssertMsg(result.pixels_changed == 0, "pixels changed: %d\n", result.pixels_changed);

	if (dst != NULL)
		gdImageDestroy(dst);
	if (p != NULL)
		gdFree(p);
	if (src != NULL)
		gdImageDestroy(src);

	return gdNumFailures();
}
