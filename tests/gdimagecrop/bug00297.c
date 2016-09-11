/**
 * Regression test for <https://github.com/libgd/libgd/issues/297>.
 *
 * We're testing that the result of gdImageCrop() is a palette image,
 * if the source was also a palette image.
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr src, dst;
    gdRect rect = {x: 0, y: 0, width: 8, height: 8};

    src = gdImageCreate(8, 8);
    dst = gdImageCrop(src, &rect);

    gdTestAssert(!gdImageTrueColor(src));
    gdTestAssert(!gdImageTrueColor(dst));

    gdImageDestroy(dst);
    gdImageDestroy(src);

    return gdNumFailures();
}
