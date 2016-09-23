/**
 * Regression test for <https://github.com/libgd/libgd/issues/309>.
 *
 * We test that an image with 64x64 pixels reports only a single chunk in the
 * GD2 image header when the chunk size is 64.
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im;
    unsigned char *buf;
    int size, word;

    im = gdImageCreate(64, 64);
    gdImageColorAllocate(im, 0, 0, 0);

    buf = gdImageGd2Ptr(im, 64, 1, &size);

    gdImageDestroy(im);

    word = buf[10] << 8 | buf[11];
    gdTestAssertMsg(word == 64, "chunk size is %d, but expected 64\n", word);
    word = buf[14] << 8 | buf[15];
    gdTestAssertMsg(word == 1, "x chunk count is %d, but expected 1\n", word);
    word = buf[16] << 8 | buf[17];
    gdTestAssertMsg(word == 1, "y chunk count is %d, but expected 1\n", word);
    gdTestAssertMsg(size == 5145, "file size is %d, but expected 5145\n", size);

    gdFree(buf);

    return gdNumFailures();
}
