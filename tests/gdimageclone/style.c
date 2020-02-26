/**
 * Cloning an image should exactly reproduce all style related data
 */


#include <string.h>
#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im, clone;
    int style[] = {0, 0, 0};

    im = gdImageCreate(8, 8);
    gdImageSetStyle(im, style, sizeof(style)/sizeof(style[0]));

    clone = gdImageClone(im);
    gdTestAssert(clone != NULL);

    gdTestAssert(clone->styleLength == im->styleLength);
    gdTestAssert(clone->stylePos == im->stylePos);
    gdTestAssert(!memcmp(clone->style, im->style, sizeof(style)/sizeof(style[0])));

    gdImageDestroy(clone);
    gdImageDestroy(im);

    return gdNumFailures();
}
