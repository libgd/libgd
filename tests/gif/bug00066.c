#include <stdio.h>
#include <gd.h>
#include <gdtest.h>

int main()
{
	gdImagePtr im;
	FILE *fp;
	char path[1024];
	int error = 0;

	sprintf(path, "%s/gif/bug00066.gif", GDTEST_TOP_DIR);
	fp = fopen(path, "rb");

	if (!fp) {
		printf("cannot open <%s>\n", path);
		return -1;
	}

	im = gdImageCreateFromGif(fp);
	fclose(fp);

	sprintf(path, "%s/gif/bug00066_exp.png", GDTEST_TOP_DIR);
	if (!gdAssertImageEqualsToFile(path, im)) {
		error = 1;
	}
	gdImageDestroy(im);

	return error;
}
