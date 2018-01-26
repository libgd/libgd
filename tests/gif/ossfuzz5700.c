#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp = gdTestFileOpen("gif/ossfuzz5700.gif");
	im = gdImageCreateFromGif(fp);
	fclose(fp);
	gdImageDestroy(im);
	return 0;
}
