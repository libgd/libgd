#include <stdio.h>
#include <gd.h>
#include <gdtest.h>


int main()
{
	gdImagePtr im;
	FILE *fp;
	char path[1024];

	sprintf(path, "%s/gif/bug00060.gif", GDTEST_TOP_DIR);
	fp = fopen(path, "rb");

	if (!fp) {
		printf("cannot open <%s>\n", path);
		return -1;
	}

	im = gdImageCreateFromGif(fp);
	fclose(fp);
	gdImageDestroy(im);
	return 0;
}
