/**
 * A most basic test for fontconfig support.
 *
 * Without actually using fontconfig, passing an empty fontlist to
 * gdImageStringFT() would return an error ("Could not find/open font").
 * We're checking that it returns NULL.
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im;
    int black;
    char *error;

    im = gdImageCreate(100, 100);
    gdTestAssert(im != NULL);
    gdImageColorAllocate(im, 255, 255, 255);
    black = gdImageColorAllocate(im, 0, 0, 0);

    gdTestAssert(gdFTUseFontConfig(1));
    error = gdImageStringFT(im, NULL, black, "", 14.0, 0.0, 10, 20, "foo");
    gdTestAssertMsg(error == NULL, "%s", error);

    gdImageDestroy(im);
    gdFontCacheShutdown();

    return gdNumFailures();
}
