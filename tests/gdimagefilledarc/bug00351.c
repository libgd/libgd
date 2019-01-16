/**
 * Test drawing of pies starting and/or ending near 90°
 *
 * See <https://github.com/libgd/libgd/issues/351>.
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im;
    int white, navy, red;

    im = gdImageCreateTrueColor(500, 500);
    white = gdTrueColorAlpha(0xFF, 0xFF, 0xFF, gdAlphaOpaque);
    navy = gdTrueColorAlpha(0x00, 0x00, 0x80, gdAlphaOpaque);
    red = gdTrueColorAlpha(0xFF, 0x00, 0x00, gdAlphaOpaque);

    gdImageFilledArc(im, 250, 250, 500, 250, 0, 88, white, gdPie);
    gdImageFilledArc(im, 250, 250, 500, 250, 88, 91 , navy, gdPie);
    gdImageFilledArc(im, 250, 250, 500, 250, 91, 360 , red, gdPie);

    gdAssertImageEqualsToFile("gdimagefilledarc/bug00351_exp.png", im);

    gdImageDestroy(im);
	return gdNumFailures();
}
