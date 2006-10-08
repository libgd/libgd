#include "gd.h"

int main()
{
 	gdImagePtr im;
	FILE *fp;
	int trans_c = gdTrueColorAlpha(255,0,0,0);

	im = gdImageCreateTrueColor(192, 36);
	gdImageColorTransparent(im, trans_c);
	gdImageFilledRectangle(im, 2,2, 80,95, 0x50FFFFFF);
	gdImageFilledRectangle(im, 0,0, 192,36, trans_c);
	fp = fopen("a.png", "wb");
 	gdImageGif(im,fp);
	fclose(fp);

 	/* Destroy it */
 	gdImageDestroy(im);

 	return 0;
}
