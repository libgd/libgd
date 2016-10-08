/**
 * Basic test of gdImageCreateFromWebp()
 *
 * We're testing that a lossless and a lossy WebP with alpha channel are
 * decoded as expected.
 */


#include "gd.h"
#include "gdtest.h"


static void test_image(const char *name)
{
    gdImagePtr im;
    FILE *fp;
    char buf[256];
    char *path;

    snprintf(buf, sizeof(buf), "%s.webp", name);
    fp = gdTestFileOpen2("webp", buf);
    im = gdImageCreateFromWebp(fp);
    gdTestAssert(im != NULL);
    fclose(fp);

    snprintf(buf, sizeof(buf), "%s_exp.png", name);
    path = gdTestFilePath2("webp", buf);
    gdAssertImageEqualsToFile(path, im);
    gdFree(path);

    gdImageDestroy(im);
}


int main()
{
    test_image("basic_lossless");
    test_image("basic_lossy");

    return gdNumFailures();
}
