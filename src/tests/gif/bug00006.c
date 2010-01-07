#include "gd.h"
#include "gdtest.h"

int main()
{
 	gdImagePtr im;
	FILE *fp;
	const r = 255, g = 0, b = 0;
	int r_f,g_f,b_f, trans_c_f;
	int trans_c = gdTrueColorAlpha(r,g,b,0);
	int error = 0;

	im = gdImageCreateTrueColor(192, 36);
	gdImageColorTransparent(im, trans_c);
	gdImageFilledRectangle(im, 0,0, 192,36, trans_c);

	fp = fopen("_tmp_bug0006.gif", "wb");
 	gdImageGif(im,fp);
	fclose(fp);

 	gdImageDestroy(im);

	fp = fopen("_tmp_bug0006.gif", "rb");
	if (!fp) {
		gdTestErrorMsg("Cannot open <%s>\n", "_tmp_bug0006.gif");
		return 1;
	}

	im = gdImageCreateFromGif(fp);
	fclose(fp);
	if (!im) {
		gdTestErrorMsg("Cannot create image from <%s>\n", "_tmp_bug0006.gif");
	}

	trans_c_f =gdImageGetTransparent(im);
		printf("c_f: %i\n", trans_c_f);
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
	unlink("_tmp_bug0006.gif");
 	return error;
}
