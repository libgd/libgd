/**
 * Test transparent support for low-depth BMP images.
 * 
 * Images with only two colors are saved as 1bbp image without compression,
 * but as 4bpp image if compression is requested.
 * 
 * Images with four to 15 colors are saved as 4bpp image, regardless whether
 * compression is requested or not.
 */

#include "gd.h"
#include "gdtest.h"

static int get_bpp(unsigned char *data)
{
    return data[0x1d] << 8 | data[0x1c];
}

static int get_comp(unsigned char *data)
{
    return data[0x21] << 24 | data[0x20] << 16 | data[0x1f] << 8 | data[0x1e];
}

int main()
{
    gdImagePtr im1, im2;
    int fg, r, g, b;
    int size;
    unsigned char *data;

    /* 2 color image */
    im1 = gdImageCreate(99, 99); /* odd pixels per row! */
    gdImageColorAllocate(im1, 0, 0, 0); /* background */
    fg = gdImageColorAllocate(im1, 255, 255, 255);
    gdImageRectangle(im1, 0, 0, 98, 98, fg); /* border to verify edge case */
    gdImageEllipse(im1, 49, 49, 75, 75, fg);

    data = gdImageBmpPtr(im1, &size, 0);
    gdTestAssert(get_bpp(data) == 1);
    gdTestAssert(get_comp(data) == 0);
    im2 = gdImageCreateFromBmpPtr(size, data);
    gdAssertImageEquals(im1, im2);

    data = gdImageBmpPtr(im1, &size, 1);
    gdTestAssert(get_bpp(data) == 4);
    gdTestAssert(get_comp(data) == 2);
    im2 = gdImageCreateFromBmpPtr(size, data);
    gdAssertImageEquals(im1, im2);

    /* 5 color image */
    r = gdImageColorAllocate(im1, 255, 0, 0);
    g = gdImageColorAllocate(im1, 0, 255, 0);
    b = gdImageColorAllocate(im1, 0, 0, 255);
    gdImageEllipse(im1, 17, 17, 28, 28, r);
    gdImageEllipse(im1, 49, 17, 28, 28, r);
    gdImageEllipse(im1, 81, 17, 28, 28, r);
    gdImageEllipse(im1, 17, 49, 28, 28, g);
    gdImageEllipse(im1, 49, 49, 28, 28, g);
    gdImageEllipse(im1, 81, 49, 28, 28, g);
    gdImageEllipse(im1, 17, 81, 28, 28, b);
    gdImageEllipse(im1, 49, 81, 28, 28, b);
    gdImageEllipse(im1, 81, 81, 28, 28, b);

    data = gdImageBmpPtr(im1, &size, 0);
    gdTestAssert(get_bpp(data) == 4);
    gdTestAssert(get_comp(data) == 0);
    im2 = gdImageCreateFromBmpPtr(size, data);
    gdAssertImageEquals(im1, im2);

    data = gdImageBmpPtr(im1, &size, 1);
    gdTestAssert(get_bpp(data) == 4);
    gdTestAssert(get_comp(data) == 2);
    im2 = gdImageCreateFromBmpPtr(size, data);
    gdAssertImageEquals(im1, im2);

    return gdNumFailures();
}
