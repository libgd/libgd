/* $Id$ */
#include "gd.h"

int main()
{
 	gdImagePtr im;
	FILE *fp;

	im = gdImageCreateTrueColor(100,100);
	
	gdImageFilledEllipse(im, 50,50, 70, 90, 0x50FFFFFF);

	fp = fopen("bug00010_out.png", "wb");
 	gdImagePng(im, fp);
	fclose(fp);

 	/* Destroy it */
 	gdImageDestroy(im);

 	return 0;
}
