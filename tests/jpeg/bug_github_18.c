#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main() {
	gdImagePtr im, imres, imexp;
	FILE *fp;
	void *pjpeg;
	char path[1024];
	unsigned int status = 0;
	int size = 0;

	snprintf(path, sizeof(path)-1, "%s/jpeg/github_bug_18.png", GDTEST_TOP_DIR);
	fp = fopen(path, "rb");
	if (!fp) {
		gdTestErrorMsg("failed, cannot open file: %s\n", path);
		return 1;
	}

	im = gdImageCreateFromPng(fp);
	fclose(fp);

	if (im == NULL) {
		gdTestErrorMsg("gdImageCreateFromJpeg failed.\n");
		return 1;
	}

	pjpeg = gdImageJpegPtr(im, &size, 100);
	if (pjpeg == NULL) {
		status = 1;
		gdTestErrorMsg("p is null\n");
		goto door2;
	}

	if (size <= 0) {
		status = 1;
		gdTestErrorMsg("size is non-positive\n");
		goto door2;
	}

	imres = gdImageCreateFromJpegPtr(size, pjpeg);
	if (imres == NULL) {
		status = 1;
		goto door2;
	}

	snprintf(path, sizeof(path)-1, "%s/jpeg/github_bug_18_exp.jpeg", GDTEST_TOP_DIR);
	fp = fopen(path, "rb");
	if (!fp) {
		gdTestErrorMsg("failed, cannot open file: %s\n", path);
		status = 1;
		goto door2;
	}

	imexp = gdImageCreateFromJpeg(fp);
	if (imexp == NULL) {
		fclose(fp);
		status = 1;
		goto door1;
	}

	snprintf(path, sizeof(path), "%s/jpeg/github_bug_18.png", GDTEST_TOP_DIR);
	if (gdTestImageCompareToImage(path, __LINE__, __FILE__, imexp, imres) != 1) {
		status = 1;
	}
	gdImageDestroy(imexp);

door1:
	gdImageDestroy(imres);

door2:
	gdFree(pjpeg);
	gdImageDestroy(im);
	return status;
}
