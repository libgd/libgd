#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

#define TMP_FN "_tmp_bug0006.gif"

int main()
{
	gdImagePtr im;
	FILE *fp;
	const int r = 255, g = 0, b = 0;
	int r_f,g_f,b_f, trans_c_f;
	int trans_c = gdTrueColorAlpha(r,g,b,0);
	int error = 0;

	im = gdImageCreateTrueColor(192, 36);
	if (im == NULL) {
		gdTestErrorMsg("Cannot create image\n");
		return 1;
	}

	gdImageColorTransparent(im, trans_c);
	gdImageFilledRectangle(im, 0,0, 192,36, trans_c);

	fp = fopen(TMP_FN, "wb");
	if (!fp) {
		gdTestErrorMsg("Cannot open <%s> for writing\n", TMP_FN);
		return 1;
	}
	gdImageGif(im,fp);
	fclose(fp);

	gdImageDestroy(im);

	fp = fopen(TMP_FN, "rb");
	if (!fp) {
		gdTestErrorMsg("Cannot open <%s> for reading\n", TMP_FN);
		return 1;
	}
	im = gdImageCreateFromGif(fp);
	fclose(fp);

	if (!im) {
		gdTestErrorMsg("Cannot create image from <%s>\n", TMP_FN);
		return 1;
	}

	trans_c_f = gdImageGetTransparent(im);
	if (gdTestAssert(trans_c_f == 1)) {
		r_f = gdImageRed(im, trans_c_f);
		g_f = gdImageGreen(im, trans_c_f);
		b_f = gdImageBlue(im, trans_c_f);

		if (!gdTestAssert(r_f == r) || !gdTestAssert(g_f==g) || !gdTestAssert(b_f==b)) {
			error = 1;
		}
	} else {
		error = 1;
	}

	/* Destroy it */
	gdImageDestroy(im);
	if (remove(TMP_FN) == -1) {
		gdTestErrorMsg("Cannot remove file: <%s>\n", TMP_FN);
	}
	return error;
}
