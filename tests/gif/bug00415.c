#include <gd.h>
#include "gdtest.h"

int main()
{
    gdImagePtr im1, im2;

    im1 = gdImageCreate(100, 100);
    gdImageColorAllocate(im1, 0, 0, 0);
    im2 = gdImageCreate(10, 10);
    gdImageColorAllocate(im2, 255, 255, 255);

    FILE *fp = gdTestTempFp();
    if (!fp) return 4;
    gdImageGifAnimBegin(im1, fp, 0, 0);
    gdImageGifAnimAdd(im1, fp, 1, 0, 0, 100, 1, NULL);
    gdImageGifAnimAdd(im2, fp, 1, 0, 0, 100, 1, im1);
    // replacing `im2` with `NULL` in the following line succeeds
    gdImageGifAnimAdd(im1, fp, 1, 0, 0, 100, 1, im2);
    gdImageGifAnimEnd(fp);
    fclose(fp);

    gdImageDestroy(im1);
    gdImageDestroy(im2);

    return 0;
}