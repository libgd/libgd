/**
 * Test that failure to convert to PNG returns NULL
 *
 * We are creating a palette image without allocating any colors in the palette,
 * and pass this image to `gdImagePngPtr()` which is supposed to fail, and as
 * such should return NULL.
 *
 * See also <https://github.com/libgd/libgd/issues/381>
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im;
    void *data;
    int size = 0;

    im = gdImageCreate(100, 100);
    gdTestAssert(im != NULL);

    data = gdImagePngPtr(im, &size);
    gdTestAssert(data == NULL);

    gdImageDestroy(im);

    return gdNumFailures();
}
