#include <stdio.h>

#include "gd.h"
#include "gdtest.h"

int main()
{
    gdImagePtr im;
    FILE *fp = gdTestFileOpen("tga/bug00084.tga");
    if (gdTestAssert(fp == NULL)) {
        return 1;
    }
    im = gdImageCreateFromTga(fp);
    fclose(fp);
    if (gdTestAssert(im != NULL)) {
        gdImageDestroy(im);
    }
    return 0;
}
