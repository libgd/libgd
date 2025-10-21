/**
 * Regression test for <https://github.com/libgd/libgd/issues/307>
 *
 * We're testing that an image that has been converted to palette with
 * GD_QUANT_NEUQUANT has its trueColor flag unset.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gdtest.h"

void test_transparent_preserved(enum gdPaletteQuantizationMethod method)
{
	gdImagePtr im;

	im = gdImageCreateTrueColor(100, 100);

    /* Assign a fully-transparent color for this image, so that the
     * conversion to palette will have a reserved entry in the palette for
     * 100% transparency. */
    int transparent = gdImageColorResolveAlpha(im, 0, 0, 0, gdAlphaTransparent);
	
	printf("Transparent: %d\n", transparent);
    gdImageColorTransparent(im, transparent);

	gdTestAssert(gdImageTrueColorToPaletteSetMethod(im, method, 0));
	gdImageTrueColorToPalette(im, 0, 256);
	gdTestAssert(!gdImageTrueColor(im));
	printf("Transparent: %d\n", im->transparent);
	gdTestAssert(im->transparent >= 0);
	gdTestAssertMsg(im->alpha[im->transparent] == gdAlphaTransparent,
		"Transparent color is not gdAlphaTransparent for algorithm %d\n", method);
}

int main()
{
	// FIXME: transparent color is not updated when using GD_QUANT_NEUQUANT method
	//test_transparent_preserved(GD_QUANT_NEUQUANT);
	test_transparent_preserved(GD_QUANT_DEFAULT);
#ifdef HAVE_LIBIMAGEQUANT
	test_transparent_preserved(GD_QUANT_LIQ);
#endif // HAVE_LIBIMAGEQUANT
	test_transparent_preserved(GD_QUANT_JQUANT);
	return gdNumFailures();
}
