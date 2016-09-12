/**
 * Regression test for <https://github.com/libgd/libgd/issues/300>
 *
 * We're testing that the resolution does not change when cloning.
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im, clone;

    im = gdImageCreate(8, 8);
    gdImageSetResolution(im, 100, 50);

    clone = gdImageClone(im);

    gdTestAssertMsg(gdImageResolutionX(clone) == gdImageResolutionX(im),
                    "horizontal resolution doesn't match: expected %d, got %d\n",
                    gdImageResolutionX(im), gdImageResolutionX(clone));
    gdTestAssertMsg(gdImageResolutionY(clone) == gdImageResolutionY(im),
                    "vertical resolution doesn't match: expected %d, got %d\n",
                    gdImageResolutionY(im), gdImageResolutionY(clone));

    gdImageDestroy(clone);
    gdImageDestroy(im);

    return gdNumFailures();
}
