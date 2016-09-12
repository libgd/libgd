/**
 * Regression test for <https://github.com/libgd/libgd/issues/299>
 *
 * We test that thick rectangles are drawn as desired.
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im;
    int black;

    im = gdImageCreate(70, 70);
    gdImageColorAllocate(im, 255, 255, 255);
    black = gdImageColorAllocate(im, 0, 0, 0);
    gdImageSetThickness(im, 4);

    gdImageRectangle(im, 10,10, 50,50, black);
    gdImageRectangle(im, 10,60, 50,60, black);
    gdImageRectangle(im, 60,10, 60,50, black);
    gdImageRectangle(im, 60,60, 60,60, black);

    gdAssertImageEqualsToFile("gdimagerectangle/bug00299_exp.png", im);

    gdImageDestroy(im);

    return gdNumFailures();
}
