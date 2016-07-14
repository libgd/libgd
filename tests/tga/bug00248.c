#include <stdio.h>

#include "gd.h"
#include "gdtest.h"

int main(int argc, char **argv)
{
    gdImagePtr im;
    FILE *fp = gdTestFileOpen("tga/bug00248.tga");
    im = gdImageCreateFromTga(fp);
    gdTestAssert(im == NULL);
    fclose(fp);
    return gdNumFailures();
}
