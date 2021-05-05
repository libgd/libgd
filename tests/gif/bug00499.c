/**
 * Test that adding identical images to GIF animations do no double-free
 *
 * We are adding two frames to a GIF animation in gdDisposalNone mode, were the
 * second frame is identical to the first, which result in that image to have
 * zero extent.  This program must not cause any memory issues.
 *
 * See also <https://github.com/libgd/libgd/issues/499>.
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im;
    int black;
    int size;
    void * res;

    im = gdImageCreate(100, 100);
    black = gdImageColorAllocate(im, 0, 0, 0);
    gdImageRectangle(im, 0, 0, 10, 10, black);

    res = gdImageGifAnimBeginPtr(im, &size, 1, 3);
    if (res != NULL) {
        gdFree(res);
    }

    res = gdImageGifAnimAddPtr(im, &size, 0, 0, 0, 100, gdDisposalNone, NULL);
    if (res != NULL) {
        gdFree(res);
    }

    res = gdImageGifAnimAddPtr(im, &size, 0, 0, 0, 100, gdDisposalNone, im);
    gdTestAssert(res == NULL);
    if (res != NULL) {
         gdFree(res);
    }

    res = gdImageGifAnimEndPtr(&size);
    if (res != NULL) {
        gdFree(res);
    }

    gdImageDestroy(im);

    return gdNumFailures();
}
