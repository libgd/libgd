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
    gdRect rect = {0, 0, 8, 8};

    src = gdImageCreate(8, 8);
    dst = gdImageCrop(src, &rect);

    gdTestAssert(!gdImageTrueColor(src));
    gdTestAssert(!gdImageTrueColor(dst));

    gdImageDestroy(dst);
    gdImageDestroy(src);

    return gdNumFailures();
}
