/**
 * Test that PNG write/read works for images exceeding libpng's default
 * user limit of 1,000,000 pixels.
 *
 * We create a 100001x1 truecolor image (100001 pixels > 100000 default),
 * write it to PNG, read it back, and verify the dimensions and pixel data.
 *
 * See also <https://github.com/libgd/libgd/issues/931>
 */

#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im, im2;
    void *png_data;
    int png_size = 0;
    int white;

    im = gdImageCreateTrueColor(100001, 1);
    gdTestAssert(im != NULL);

    white = gdImageColorAllocate(im, 255, 255, 255);
    gdImageSetPixel(im, 50000, 0, white);

    png_data = gdImagePngPtr(im, &png_size);
    gdTestAssert(png_data != NULL);
    gdTestAssertMsg(png_size > 0, "Expected non-zero PNG data size");

    im2 = gdImageCreateFromPngPtr(png_size, png_data);
    gdTestAssertMsg(im2 != NULL, "Expected to read back the large PNG");
    if (im2 != NULL) {
        gdTestAssertMsg(im2->sx == 100001, "Width mismatch");
        gdTestAssertMsg(im2->sy == 1, "Height mismatch");
        gdTestAssertMsg(gdImageTrueColorPixel(im2, 50000, 0) == 0xFFFFFF,
                        "Expected white pixel at (50000,0)");
        gdImageDestroy(im2);
    }

    gdFree(png_data);
    gdImageDestroy(im);

    return gdNumFailures();
}
