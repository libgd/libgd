/* $Id$ */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main()
{
	int error;
	gdImagePtr im;
	FILE *fp;
	char path[1024];
	const char * files[2] = {"bug00088_1.png", "bug00088_2.png"};
	const char * files_exp[2] = {"bug00088_1_exp.png", "bug00088_2_exp.png"};

	int i, cnt = 2;
	error = 0;

	for (i = 0; i < cnt; i++) {

		sprintf(path, "%s/png/%s", GDTEST_TOP_DIR, files[i]);
		fp = fopen(path, "rb");
		if (!fp) {
			printf("failed, cannot open file <%s>\n", path);
			return 1;
		}

		im = gdImageCreateFromPng(fp);
		fclose(fp);

		if (!im) {
			error |= 1;
			continue;
		}

		sprintf(path, "%s/png/%s", GDTEST_TOP_DIR, files_exp[i]);
		if (!gdAssertImageEqualsToFile(path, im)) {
			error |= 1;
		}
		gdImageDestroy(im);
	}

	return error;
}
