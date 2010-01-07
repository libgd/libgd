
#include "gdtest.h"
#include "gd.h"

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
	char path[1024];

	for (i=0; i < files_cnt; i++) {
		sprintf(path, "%s/gif/%s", GDTEST_TOP_DIR, giffiles[i]);

		fp = fopen(path, "rb");
		if (!fp) {
			gdTestErrorMsg("<%s> Input file does not exist!\n", path);
			return 1;
		}

		im = gdImageCreateFromGif(fp);
		fclose(fp);

		if (valid[i]) {
			if (!im) {
				error = 1;
			} else {
				sprintf(path, "%s/gif/%s", GDTEST_TOP_DIR, exp[i]);
				if (!gdAssertImageEqualsToFile(path, im)) {
					error = 1;
				}
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
