#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	char *file;
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

	file = gdTestTempFile("bug0006.gif");
	fp = fopen(file, "wb");
	if (!fp) {
		gdTestErrorMsg("Cannot open <%s> for writing\n", file);
		return 1;
	}

	gdImageTrueColorToPaletteSetMethod(im, GD_QUANT_JQUANT, 0);
	gdImageTrueColorToPalette(im, 1, 256);

	gdImageGif(im,fp);
	fclose(fp);

	gdImageDestroy(im);

	fp = fopen(file, "rb");
	if (!fp) {
		gdTestErrorMsg("Cannot open <%s> for reading\n", file);
		return 1;
	}
	im = gdImageCreateFromGif(fp);
	fclose(fp);

	if (!im) {
		gdTestErrorMsg("Cannot create image from <%s>\n", file);
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
	free(file);
	return error;
}
