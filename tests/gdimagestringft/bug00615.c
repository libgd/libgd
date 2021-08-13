/**
 * Test that rendering an empty string does not fail
 *
 * Rendering an empty string with gdImageStringFT() is not supposed to fail;
 * it is just a no-op.
 *
 * See <https://github.com/libgd/libgd/issues/615>
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
    gdImagePtr im = gdImageCreate(100, 100);

    int rect[8];
    int fg = gdImageColorAllocate(im, 255, 255, 255);
    char *path = gdTestFilePath("freetype/DejaVuSans.ttf");
    char *res = gdImageStringFT(im, rect, fg, path, 12, 0, 10, 10, "");

    gdTestAssert(res == NULL);
    free(path);
    gdImageDestroy(im);
    return gdNumFailures();
}
