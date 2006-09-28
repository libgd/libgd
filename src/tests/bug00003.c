#include "gd.h"

int main()
{
 	gdImagePtr im;
	FILE *fp;


	im = gdImageCreateTrueColor(100,100);
	gdImageRectangle(im, 2,2, 80,95, 0x50FFFFFF);
	fp = fopen("a.png", "wb");
 	/* Write img to stdout */
 	gdImagePng(im,fp);
	fclose(fp);

 	/* Destroy it */
 	gdImageDestroy(im);

 	return 0;
}
