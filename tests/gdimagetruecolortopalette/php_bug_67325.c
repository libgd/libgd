/**
 * Regression test for <https://bugs.php.net/bug.php?id=67325>
 *
 * We convert a truecolor image to a palette image and check that no duplicated
 * white colors are in the resulting palette.
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im;
    FILE *fp;
    int i, white;

    fp = gdTestFileOpen2("gdimagetruecolortopalette", "php_bug_67325.jpeg");
    gdTestAssert(fp != NULL);
    im = gdImageCreateFromJpeg(fp);
    gdTestAssert(im != NULL);
    gdTestAssert(!fclose(fp));

    gdTestAssert(gdImageTrueColorToPalette(im, 0, 256));

    white = 0;
    for (i = 0; i < im->colorsTotal; i++) {
        if (im->red[i] == 255 && im->green[i] == 255 && im->blue[i] == 255) {
            white++;
        }
    }
    gdTestAssertMsg(white <= 1, "At most one white color palette entry expected, got %d\n", white);

    gdImageDestroy(im);

    return gdNumFailures();
}
