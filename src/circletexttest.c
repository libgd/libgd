#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <stdio.h>
#include "gd.h"

int main(void)
{
	/* 2.0.22: can't depend on PNG either  */
	char *error;
#ifdef HAVE_LIBJPEG
	FILE *in = 0;
#endif
	FILE *out;
	gdImagePtr im;
	int radius;
	/* Create an image of text on a circle, with an
	 * alpha channel so that we can copy it onto a
	 * background
	 * TBB: 2.0.18: shouldn't depend on JPEG
	 */
#ifdef HAVE_LIBJPEG
	in = fopen("eleanor.jpg", "rb");
	if(!in) {
		im = gdImageCreateTrueColor(300, 300);
	} else {
		im = gdImageCreateFromJpeg(in);
		fclose(in);
	}
#else
	im = gdImageCreateTrueColor(300, 300);
#endif /* HAVE_LIBJPEG */
	if(!im) {
		fprintf(stderr, "gdImageCreateTrueColor failed \n");
		return 1;
	}
	if(gdImageSX(im) < gdImageSY(im)) {
		radius = gdImageSX(im) / 2;
	} else {
		radius = gdImageSY(im) / 2;
	}

	error = gdImageStringFTCircle(im,
	                              gdImageSX(im) / 2, gdImageSY(im) / 2,
	                              radius, radius / 2,
	                              0.8, "arial", 24, "top text", "bottom text",
	                              gdTrueColorAlpha(192, 100, 255, 32)
	                             );
	if(error)  {
		fprintf(stderr, "gdImageStringFTEx error: %s\n", error);
	}

	out = fopen("gdfx.png", "wb");
	if(!out) {
		fprintf(stderr, "Can't create gdfx.png\n");
		return 1;
	}

	gdImagePng(im, out);
	fclose(out);
	gdImageDestroy(im);
	return 0;
}
