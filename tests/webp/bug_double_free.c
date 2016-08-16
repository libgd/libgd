/**
 * Test that a too large image doesn't trigger an double-free when written
 * to memory.
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im1, im2;
    FILE *fp;
    int size;

    fp = gdTestFileOpen2("webp", "bug_double_free.jpg");
    gdTestAssert(fp != NULL);
    im1 = gdImageCreateFromJpeg(fp);
    gdTestAssert(im1 != NULL);
    fclose(fp);

    im2 = gdImageWebpPtr(im1, &size);
    gdTestAssert(im2 == NULL);

    gdImageDestroy(im1);

    return gdNumFailures();
}
