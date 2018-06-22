/**
 * Test that gdImageCrop() retains transparency
 *
 * We create an image with transparent pixels, crop the image, and check whether
 * all pixels are identical to the respective source image.
 *
 * See <https://github.com/libgd/libgd/issues/432>
 */


#include "gd.h"
#include "gdtest.h"


#define WIDTH 10
#define HEIGHT 10
#define XOFF 1
#define YOFF 1


int main()
{
    gdImagePtr src, dst;
    gdRect crop = {XOFF, YOFF, WIDTH-XOFF-1, HEIGHT-YOFF-1};
    int i, j;

    src = gdImageCreateTrueColor(WIDTH, HEIGHT);
    gdImageAlphaBlending(src, gdEffectReplace);
    gdImageFilledRectangle(src, 0, 0, WIDTH-1, HEIGHT-1,
            gdTrueColorAlpha(gdRedMax, gdGreenMax, gdBlueMax, gdAlphaMax));

    dst = gdImageCrop(src, &crop);
    gdTestAssert(dst != NULL);

    for (i = 0; i < gdImageSX(dst); i++) {
        for (j = 0; j < gdImageSY(dst); j++) {
            gdTestAssert(gdImageGetPixel(dst, i, j) == gdImageGetPixel(src, i+XOFF, j+YOFF));
        }
    }

    gdImageDestroy(src);
    gdImageDestroy(dst);

    return gdNumFailures();
}
