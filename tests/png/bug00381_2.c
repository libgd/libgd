/**
 * Test that failure to convert to PNG returns NULL
 *
 * We are reading a palette image without any colors in the palette, and pass
 * this image to `gdImagePngPtr()` which is supposed to fail, and as such should
 * return NULL.
 *
 * See also <https://github.com/libgd/libgd/issues/381>
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im;
    FILE *fp;
    void *data;
    int size = 0;

    fp = gdTestFileOpen2("png", "bug00381_2.gd");
    gdTestAssert(fp != NULL);
    im = gdImageCreateFromGd(fp);
    gdTestAssert(im != NULL);
    fclose(fp);

    data = gdImagePngPtr(im, &size);
    gdTestAssert(data == NULL);

    gdImageDestroy(im);

    return gdNumFailures();
}
