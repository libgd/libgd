#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main()
{
	int error;
	gdImagePtr im;
	FILE *fp;
	char *path;
	const char * files[2] = {"bug00088_1.png", "bug00088_2.png"};
	const char * files_exp[2] = {"bug00088_1_exp.png", "bug00088_2_exp.png"};

	int i, cnt = 2;
	error = 0;

	for (i = 0; i < cnt; i++) {

		fp = gdTestFileOpen2("png", files[i]);
		im = gdImageCreateFromPng(fp);
		fclose(fp);

		if (!im) {
			error |= 1;
			continue;
		}

		path = gdTestFilePath2("png", files_exp[i]);

		if (!gdAssertImageEqualsToFile(path, im)) {
			error |= 1;
		}
		free(path);
		gdImageDestroy(im);
	}

	return error;
}
