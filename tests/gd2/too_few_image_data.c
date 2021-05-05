/*
too_few_image_data.gd2 claims to have a size of 12336x48 pixels, but doesn't
provide as much image data. We test that gdImageCreateFromGd2Ctx() returns NULL
in this case.
*/

#include "gd.h"
#include "gdtest.h"

int main()
{
    gdImagePtr im;
    FILE *fp;

    fp = gdTestFileOpen2("gd2", "too_few_image_data.gd2");
    gdTestAssert(fp != NULL);
    im = gdImageCreateFromGd2(fp);
    gdTestAssert(im == NULL);
    fclose(fp);

    return gdNumFailures();
}
