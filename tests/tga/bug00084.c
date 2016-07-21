#include <stdio.h>

#include "gd.h"
#include "gdtest.h"

int main()
{
    gdImagePtr im;
    FILE *fp = gdTestFileOpen("tga/bug00084.tga");
    im = gdImageCreateFromTga(fp);
	gdImageDestroy(im);
    return 0;
}
