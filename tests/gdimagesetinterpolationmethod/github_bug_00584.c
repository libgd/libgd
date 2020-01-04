/**
 * Test that gdImageSetInterpolationMethod(im, GD_DEFAULT) is consistent
 *
 * See <https://github.com/libgd/libgd/issues/584>
 */

#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im;
    gdInterpolationMethod old_m, new_m;
    interpolation_method old_f, new_f;

    im = gdImageCreateTrueColor(8, 8);
    gdTestAssert(im != NULL);
    gdTestAssert(gdImageSetInterpolationMethod(im, GD_SINC));
    old_m = gdImageGetInterpolationMethod(im);
    gdTestAssert(old_m == GD_SINC);
    old_f = im->interpolation;
    gdTestAssert(gdImageSetInterpolationMethod(im, GD_DEFAULT));
    new_m = gdImageGetInterpolationMethod(im);
    gdTestAssert(new_m == GD_LINEAR);
    new_f = im->interpolation;
    gdTestAssert(new_m != old_m);
    gdTestAssert(new_f != old_f);
    gdImageDestroy(im);
    return gdNumFailures();
}
