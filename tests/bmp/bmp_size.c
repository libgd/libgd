/*
 * Test that the bfSize and biSizeImage fields of the bitmap header
 * match the actual file size.
 * 
 * bfSize and biSizeImage are 32 bit unsigned int in little-endian format.
 */

#include "gd.h"
#include "gdtest.h"

#define OFFSET_BF_SIZE 2
#define OFFSET_BI_SIZE_IMAGE 34
#define BIMAP_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define PALETTE_SIZE 4
#define HEADER_SIZE (BIMAP_HEADER_SIZE + INFO_HEADER_SIZE + PALETTE_SIZE)

unsigned int get_field(unsigned char *data, int offset)
{
    return data[offset + 3] << 24 | data[offset + 2] << 16 | data[offset + 1] << 8 | data[offset];
}

int main()
{
    gdImagePtr im;
    unsigned char *data;
    int size;
    unsigned int bfSize, biSizeImage;

    im = gdImageCreate(19, 19);
    gdImageColorAllocate(im, 0, 0, 0); // bg

    data = gdImageBmpPtr(im, &size, 0);
    gdTestAssert(data != NULL);
    gdTestAssert(data[0] == 'B' && data[1] == 'M');
    bfSize = get_field(data, OFFSET_BF_SIZE);
    gdTestAssertMsg(bfSize == size, "expected %d, got %u", size, bfSize);
    biSizeImage = get_field(data, OFFSET_BI_SIZE_IMAGE);
    gdTestAssertMsg(biSizeImage == size - HEADER_SIZE, "expected %d, got %u", size - HEADER_SIZE, biSizeImage);

    data = gdImageBmpPtr(im, &size, 1);
    gdTestAssert(data != NULL);
    gdTestAssert(data[0] == 'B' && data[1] == 'M');
    bfSize = get_field(data, OFFSET_BF_SIZE);
    gdTestAssertMsg(bfSize == size, "expected %d, got %u", size, bfSize);
    biSizeImage = get_field(data, OFFSET_BI_SIZE_IMAGE);
    gdTestAssertMsg(biSizeImage == size - HEADER_SIZE, "expected %d, got %u", size - HEADER_SIZE, biSizeImage);

    gdImageDestroy(im);
    return gdNumFailures();
}
