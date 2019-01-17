/**
 * Test that failure to convert to JPEG returns NULL
 *
 * We are creating an image, set its width to zero, and pass this image to
 * `gdImageJpegPtr()` which is supposed to fail, and as such should return NULL.
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

    src->sx = 0; /* this hack forces gdImageJpegPtr() to fail */

    dst = gdImageJpegPtr(src, &size, 0);
    gdTestAssert(dst == NULL);

    gdImageDestroy(src);

    return gdNumFailures();
}
