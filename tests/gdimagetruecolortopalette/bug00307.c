/**
 * Regression test for <https://github.com/libgd/libgd/issues/307>
 *
 * We're testing that an image that has been converted to palette with
 * GD_QUANT_NEUQUANT has its trueColor flag unset.
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
	gdImagePtr im;

	im = gdImageCreateTrueColor(100, 100);

	gdTestAssert(gdImageTrueColorToPaletteSetMethod(im, GD_QUANT_NEUQUANT, 0));
	gdImageTrueColorToPalette(im, 0, 256);
	gdTestAssert(!gdImageTrueColor(im));

	gdImageDestroy(im);

	return gdNumFailures();
}
