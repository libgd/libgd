/**
 * Passing an unrecognized format to gdImageGd2() should result in
 * GD2_FMT_TRUECOLOR_COMPRESSED for truecolor images.
 *
 * See <https://github.com/libgd/libgd/issues/289>.
 */

#include "gd.h"
#include "gdtest.h"


#define GD2_FMT_UNRECOGNIZED 0
#define GD2_FMT_TRUECOLOR_COMPRESSED 4

#define MSG "expected %s byte to be %d, but got %d\n"


int main()
{
    gdImagePtr im;
    char *buffer;
    int size;

    im = gdImageCreateTrueColor(10, 10);
    gdTestAssert(im != NULL);
    buffer = (char *) gdImageGd2Ptr(im, 128, GD2_FMT_UNRECOGNIZED, &size);
    gdTestAssert(buffer != NULL);
    gdImageDestroy(im);
    gdTestAssertMsg(buffer[12] == 0, MSG, "1st", 0, buffer[12]);
    gdTestAssertMsg(buffer[13] == GD2_FMT_TRUECOLOR_COMPRESSED, MSG, "2nd", GD2_FMT_TRUECOLOR_COMPRESSED, buffer[13]);

    gdFree(buffer);

    return gdNumFailures();
}
