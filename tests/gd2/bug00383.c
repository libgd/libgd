/**
 * Test that invalid transparent colors can't be read
 *
 * We're reading a corrupt palette image in GD2 format, which has only a single
 * palette entry, but claims that the transparent color would be 1.  We check
 * that the transparency is simply ignored in this case.
 *
 * See also <https://github.com/libgd/libgd/issues/383>
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im;
    FILE *fp;

    fp = gdTestFileOpen2("gd2", "bug00383.gd2");
    gdTestAssert(fp != NULL);
    im = gdImageCreateFromGd2(fp);
    gdTestAssert(im != NULL);
    fclose(fp);

    gdTestAssert(gdImageGetTransparent(im) == -1);

    gdImageDestroy(im);

    return gdNumFailures();
}
