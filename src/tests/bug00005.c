#include "gd.h"

int main()
{
 	gdImagePtr im;
	char * giffiles[3] = {"bug00005_1.gif", "bug00005_2.gif", "bug00005_3.gif"};

	FILE *fp;
	int i = 0;
	const int files_cnt = 3;

	for (i=0; i < files_cnt; i++) {
		fp = fopen(giffiles[i], "rb");
		if (!fp) {
			fprintf(stderr, "Input file does not exist!\n");
			return 1;
		}

		im = gdImageCreateFromGif(fp);

		if (!im) {
			fprintf(stderr, "%s Invalid GIF file\n", giffiles[i]);
		} else {
			fprintf(stderr, "%s valid GIF file\n", giffiles[i]);
		 	gdImageDestroy(im);
		}
	}

 	return 0;
}
