#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <gd.h>

#include "gdtest.h"

int main()
{
 	gdImagePtr dst_tc, src;
	int c0,c1;

	src = gdImageCreate(5,5);
	gdImageAlphaBlending(src, 0);

	c0 = gdImageColorAllocate(src, 255,255,255);
	c1 = gdImageColorAllocateAlpha(src, 255,0,0,70);

	gdImageFilledRectangle(src, 0,0, 4,4, c1);

	dst_tc  = gdImageCreateTrueColor(5,5);
	gdImageAlphaBlending(dst_tc, 0);
	gdImageCopy(dst_tc, src, 0,0, 0,0, gdImageSX(src), gdImageSY(src));

	//CuAssertImageEquals(tc, src, dst_tc);

 	/* Destroy it */
 	gdImageDestroy(dst_tc);
	gdImageDestroy(src);
	return 0;
}
