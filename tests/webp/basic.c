/**
 * Basic test of gdImageCreateFromWebp()
 *
 * We're testing that a lossless and a lossy WebP with alpha channel are
 * decoded as expected. For the lossy image we allow at most 1% of the pixels
 * to be different.
 */


#include "gd.h"
#include "gdtest.h"


static gdImagePtr read_webp(const char *name)
{
    gdImagePtr im;
    FILE *fp;
    char buf[256];

    snprintf(buf, sizeof(buf), "%s.webp", name);
    fp = gdTestFileOpen2("webp", buf);
    im = gdImageCreateFromWebp(fp);
    gdTestAssert(im != NULL);
    fclose(fp);

    return im;
}


static void test_lossless_image(const char *name)
{
    gdImagePtr im;
    char buf[256];
    char *path;

    im = read_webp(name);

    snprintf(buf, sizeof(buf), "%s_exp.png", name);
    path = gdTestFilePath2("webp", buf);
    gdAssertImageEqualsToFile(path, im);
    gdFree(path);

    gdImageDestroy(im);
}


static void test_lossy_image(const char *name)
{
    gdImagePtr im, exp;
    FILE *fp;
    char buf[256];
    CuTestImageResult result = {0, 0};
    unsigned threshold;

    im = read_webp(name);

    snprintf(buf, sizeof(buf), "%s_exp.png", name);
    fp = gdTestFileOpen2("webp", buf);
    exp = gdImageCreateFromPng(fp);
    gdTestAssert(im != NULL);
    fclose(fp);

    gdTestImageDiff(im, exp, NULL, &result);
    threshold = (unsigned) (gdImageSX(im) * gdImageSY(im) / 100);
    gdTestAssertMsg(result.pixels_changed < threshold,
                    "%d pixels are different", result.pixels_changed);

    gdImageDestroy(exp);
    gdImageDestroy(im);
}


int main()
{
    test_lossless_image("basic_lossless");
    test_lossy_image("basic_lossy");

    return gdNumFailures();
}
