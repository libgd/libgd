#include "gd.h"
#include <stdio.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im, tile;
	int im_black, tile_black;
	int x,y, error = 0;

/*	fputs("flag 0\n", stdout); */
	im = gdImageCreate(150, 150);
	tile = gdImageCreate(36, 36);
	gdImageColorAllocate(tile,255,255,255); /* allocate white for background color */
	tile_black = gdImageColorAllocate(tile,0,0,0);
	gdImageColorAllocate(im,255,255,255); /* allocate white for background color */
	im_black = gdImageColorAllocate(im,0,0,0);


	/* create the dots pattern */
	for (x=0; x<36; x+=2) {
		for (y=0; y<36; y+=2) {
			gdImageSetPixel(tile,x,y,tile_black);
		}
	}

	gdImageSetTile(im,tile);
	gdImageRectangle(im, 9,9,139,139, im_black);
	gdImageLine(im, 9,9,139,139, im_black);
	gdImageFill(im, 11,12, gdTiled);


/*	fputs("flag 1\n", stdout); */
	gdImageFill(im, 0, 0, 0xffffff);
/*	fputs("flag 2\n", stdout); */
	gdImageFill(im, 0, 0, 0xffffff);
/*	fputs("flag 2\n", stdout); */


	if (!gdAssertImageEqualsToFile("gdimagefill/bug00002_2_exp.png", im)) {
		error = 1;
	}

	/* Destroy it */
	gdImageDestroy(im);
	gdImageDestroy(tile);
	return error;
}
