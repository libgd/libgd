#include "gd.h"

int main()
{
 	gdImagePtr dst_tc, src;
	FILE *fp;
	int c0,c1,c3;

	src  = gdImageCreate(5,5);
	c0 = gdImageColorAllocate(src, 255,255,255);
	c1 = gdImageColorAllocateAlpha(src, 255,0,0,70);

	gdImageAlphaBlending(src, 0);
	gdImageFill(src, 0,0, c1);

	dst_tc  = gdImageCreateTrueColor(5,5);
	gdImageAlphaBlending(dst_tc, 0);

	gdImageCopy(dst_tc, src, 0,0, 0,0, gdImageSX(src), gdImageSY(src));

	fp = fopen("a.png", "wb");
 	gdImagePng(dst_tc, fp);
	fclose(fp);

 	/* Destroy it */
 	gdImageDestroy(src);
 	gdImageDestroy(dst_tc);

 	return 0;
}
