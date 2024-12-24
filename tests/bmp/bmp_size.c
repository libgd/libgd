/*
 * Test that the bfSize field of the bitmap header matches the actual file size.
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
    gdImagePtr im;
    unsigned char *data;
    int size;
    unsigned int bfSize;

    im = gdImageCreate(19, 19);
    gdImageColorAllocate(im, 0, 0, 0); // bg
    data = gdImageBmpPtr(im, &size, 0);
    gdTestAssert(data != NULL);
    gdTestAssert(data[0] == 'B' && data[1] == 'M');
    /* bfSize is a 32 bit unsigned int in little-endian representation */
    bfSize = data[5] << 24 | data[4] << 16 | data[3] << 8 | data[2];
    gdTestAssertMsg(bfSize == size, "expected %d, got %u", size, bfSize);
    gdImageDestroy(im);
    return gdNumFailures();
}
