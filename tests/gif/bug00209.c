/* Test case for  <https://github.com/libgd/libgd/issues/209>. */

#include "gd.h"
#include "gdtest.h"

int main()
{
    gdImagePtr im;
    FILE *fp;

    /* printf("start\n"); */

    fp = gdTestFileOpen("gif/bug00209.gd2");
    gdTestAssert(fp != NULL);
    im = gdImageCreateFromGd2(fp);
    gdTestAssert(im != NULL);
    fclose(fp);
    /* printf("loaded\n"); */

    fp = gdTestTempFp();
    gdTestAssert(fp != NULL);
    gdImageGif(im, fp);
    fclose(fp);
    /* printf("saved\n"); */

    gdImageDestroy(im);

    return gdNumFailures();
}
