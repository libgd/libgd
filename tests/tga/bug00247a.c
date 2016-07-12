/*
We test that a 8bpp TGA file will be gracefully rejected by
gdImageCreateFromTga().
*/

#include <stdio.h>

#include "gd.h"
#include "gdtest.h"

int main(int argc, char **argv)
{
    gdImagePtr im;
    FILE *fp = gdTestFileOpen("tga/bug00247a.tga");
    im = gdImageCreateFromTga(fp);
    gdTestAssert(im == NULL);
    fclose(fp);
    return gdNumFailures();
}
