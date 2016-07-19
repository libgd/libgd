/* See also <https://github.com/libgd/libgd/pull/257> */

#include <stdio.h>

#include "gd.h"
#include "gdtest.h"

int main()
{
    gdImagePtr im;
    FILE *fp = gdTestFileOpen("tga/bug00248a.tga");
    im = gdImageCreateFromTga(fp);
    gdTestAssert(im == NULL);
    fclose(fp);
    return gdNumFailures();
}
