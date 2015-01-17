/* $Id$ */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	char path[2048];
	FILE *fp;

	im = gdImageCreateTrueColor(2,2);
	if (!im) {
		return 0;
	}

	sprintf(path, "%s/webp/tmpbug00111.webp", GDTEST_TOP_DIR);

	fp = fopen(path, "wb");
    gdImageWebp(im, fp);
    fclose(fp);

	unlink(path);
	gdImageDestroy(im);
    return 0;
}
