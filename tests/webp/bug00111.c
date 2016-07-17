#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
# include <unistd.h>
#endif
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;

	im = gdImageCreateTrueColor(20, 20);
	if (!im) {
		return 0;
	}

	fp = gdTestTempFp();
    gdImageWebp(im, fp);
    fclose(fp);

	gdImageDestroy(im);
    return 0;
}
