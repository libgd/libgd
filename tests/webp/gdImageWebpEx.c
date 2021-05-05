/**
 * Basic testcase for gdImageWebpEx()
 **/
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp1;
	int size = 1;

	im = gdImageCreateTrueColor(20, 20);
	if (!im) {
		return 1;
	}

	fp1 = gdTestTempFp();
	gdImageWebpEx(im, fp1, 8);
	gdImageWebpPtrEx(im, &size, 9);

	fclose(fp1);

	gdImageDestroy(im);
	return 0;
}
