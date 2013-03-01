#include "gd.h"
#include <stdio.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	char path[1024];
	FILE* fp;

	sprintf(path, "%s/gd/crafted_num_colors.gd", GDTEST_TOP_DIR);

	fp = fopen(path, "rb");
	if (!fp) {
		return 1;
	}
	im = gdImageCreateFromGd(fp);
	fclose(fp);
	if (im) {
		gdImageDestroy(im);
		return 1;
	}
	return 0;
}
