/**
  * Basic test for gdImageColorClosestHWB()
  **/
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;

	fp = gdTestFileOpen2("gdimageflip", "remi.png");
	im = gdImageCreateFromPng(fp);
	fclose(fp);

	int result = gdImageColorClosestHWB(im, 255, 0, 255);

	if (gdTestAssert(result <= 0))
	{
		gdImageDestroy(im);
		return 1;
	}

	gdImageDestroy(im);
	return 0;
}
