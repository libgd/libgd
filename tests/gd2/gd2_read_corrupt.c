/* Just try to read the invalid gd2 image & not crash. */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	char path[1024];

	/* Read the corrupt image. */
	sprintf(path, "%s/gd2/invalid_neg_size.gd2", GDTEST_TOP_DIR);
	fp = fopen(path, "rb");
	if (!fp) {
		printf("failed, cannot open file\n");
		return 1;
	}
	im = gdImageCreateFromGd2(fp);
	fclose(fp);

	/* Should have failed & rejected it. */
	return im == NULL ? 0 : 1;
}
