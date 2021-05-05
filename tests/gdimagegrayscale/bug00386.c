/**
 * Test that gdImageGrayScale() produces pure gray-scale images
 *
 * We are reading a PNG image with transparency, apply gdImageGrayScale() and
 * test that each pixel has equal red, blue and green components. To avoid
 * flooding the test log, we bail out after the first non gray-scale pixel.
 *
 * We also make sure that the image is a truecolor image, since otherwise it is
 * not guaranteed that all pixels are actually gray-scale.
 *
 * See also <https://github.com/libgd/libgd/issues/386>.
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im;
    FILE *fp;
    int res;
    int i, j;

    fp = gdTestFileOpen2("gdimagegrayscale", "bug00386.png");
    gdTestAssert(fp != NULL);
    im = gdImageCreateFromPng(fp);
    gdTestAssert(im != NULL);
    fclose(fp);

    res = gdImageTrueColor(im);
    gdTestAssert(res != 0);

    res = gdImageGrayScale(im);
    gdTestAssert(res != 0);

    for (i = 0; i < gdImageSX(im); i++) {
        for (j = 0; j < gdImageSY(im); j++) {
            int color = gdImageGetTrueColorPixel(im, i, j);
            int red = gdImageRed(im, color);
            int green = gdImageGreen(im, color);
            int blue = gdImageBlue(im, color);
            if (!gdTestAssert(red == green && green == blue)) {
                return gdNumFailures();
            }
        }
    }

    gdImageDestroy(im);

    return gdNumFailures();
}
