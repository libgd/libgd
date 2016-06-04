#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	char * giffiles[4] = {"bug00005_0.gif", "bug00005_1.gif", "bug00005_2.gif", "bug00005_3.gif"};
	int valid[4]       = {0, 0, 0, 0};
	char *exp[4] = {NULL, NULL, "bug00005_2_exp.png", NULL};
	const int files_cnt = 4;
	FILE *fp;
	int i = 0;
	int error = 0;
	char *path;

	for (i=0; i < files_cnt; i++) {
		fp = gdTestFileOpen2("gif", giffiles[i]);
		im = gdImageCreateFromGif(fp);
		fclose(fp);

		if (valid[i]) {
			if (!im) {
				error = 1;
			} else {
				path = gdTestFilePath2("gif", exp[i]);
				if (!gdAssertImageEqualsToFile(path, im)) {
					error = 1;
				}
				free(path);
				gdImageDestroy(im);
			}
		} else {
			if (!gdTestAssert(im == NULL)) {
				error = 1;
			}
		}
	}

	return error;
}
