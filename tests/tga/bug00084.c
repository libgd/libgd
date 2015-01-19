#include <stdio.h>

#include "gd.h"
#include "gdtest.h"

int main(int argc, char **argv)
{
    gdImagePtr im;
    FILE *fp;
    fp = fopen(GDTEST_TOP_DIR "/tga/bug00084.tga", "rb");
    if (!fp) {
		printf("cannot read tga %s\n", GDTEST_TOP_DIR "/tga/bug00084.tga");
		return 1;
	}
    im = gdImageCreateFromTga(fp);
	gdImageDestroy(im);
    return 0;
}
