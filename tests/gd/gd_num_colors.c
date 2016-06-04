#include "gd.h"
#include <stdio.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp = gdTestFileOpen("gd/crafted_num_colors.gd");
	im = gdImageCreateFromGd(fp);
	fclose(fp);
	if (im) {
		gdImageDestroy(im);
		return 1;
	}
	return 0;
}
