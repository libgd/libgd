/**
 * Basic test case for reading TGA files.
 */


#include "gd.h"
#include "gdtest.h"


static void assert_equals(char *exp, char *orig);


int main()
{
    assert_equals("tga_read_rgb.png", "tga_read_rgb.tga");
    assert_equals("tga_read_rgb.png", "tga_read_rgb_rle.tga");

    return gdNumFailures();
}


static void assert_equals(char *exp, char *orig)
{
    gdImagePtr im;
    FILE *fp;
    char *filename;

    fp = gdTestFileOpen2("tga", orig);
    gdTestAssertMsg(fp != NULL, "can't open %s", orig);

    im = gdImageCreateFromTga(fp);
    gdTestAssertMsg(im != NULL, "can't read %s", orig);
    fclose(fp);

    filename = gdTestFilePath2("tga", exp);
    gdAssertImageEqualsToFile(filename, im);
    gdFree(filename);

    gdImageDestroy(im);
}
