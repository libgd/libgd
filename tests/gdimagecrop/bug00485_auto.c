/**
 * Test that gdImageCropAuto() works pixel precise
 *
 * We test that a single black pixel anywhere (but the corners) on an 8x8 pixel
 * image with a white background is auto-cropped to a 1x1 pixel image with a
 * black pixel.
 *
 * See <https://github.com/libgd/libgd/issues/485>
 */


#include "gd.h"
#include "gdtest.h"


gdImagePtr createWhiteImageWithBlackPixelAt(int x, int y)
{
    gdImagePtr im = gdImageCreateTrueColor(8, 8);
    gdImageFilledRectangle(im, 0, 0, 7, 7, 0xffffff);
    gdImageSetPixel(im, x, y, 0x000000);
    return im;
}


int main()
{
    int x, y, height, width, color;
    gdImagePtr orig, cropped;

    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            if ((x == 0 && (y == 0 || y == 7)) || (x == 7 && (y == 0 || y == 7))) {
                continue; // skip the corners
            }
            orig = createWhiteImageWithBlackPixelAt(x, y);
            cropped = gdImageCropAuto(orig, GD_CROP_SIDES);
            gdTestAssertMsg(cropped != NULL, "Pixel at %d, %d: unexpected NULL crop\n", x, y);
            if (cropped) {
                width = gdImageSX(cropped);
                gdTestAssertMsg(width == 1, "Pixel at %d, %d: unexpected width (%d)\n", x, y, width);
                height = gdImageSY(cropped);
                gdTestAssertMsg(height == 1, "Pixel at %d, %d: unexpected height (%d)\n", x, y, height);
                color = gdImageGetPixel(cropped, 0, 0);
                gdTestAssertMsg(color == 0x000000, "Pixel at %d, %d: unexpected color (%d)\n", x, y, color);
                gdImageDestroy(cropped);
            }
            gdImageDestroy(orig);
        }
    }

    return gdNumFailures();
}
