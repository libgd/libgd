#include <stdio.h>
#include <stdlib.h>
#include <gd.h>

int main(void)
{
	int i;
	FILE * out;

	gdImagePtr im;
	gdImagePtr prev =NULL;
	int black;

	im = gdImageCreate(100, 100);
	if (!im) {
		fprintf(stderr, "can't create image");
		return 1;
	}

	out = fopen("anim.gif", "wb");
	if (!out) {
		fprintf(stderr, "can't create file %s", "anim.gif");
		return 1;
	}

	gdImageColorAllocate(im, 255, 255, 255); /* allocate white as side effect */
	gdImageGifAnimBegin(im, out, 1, -1);

	for(i = 0; i < 20; i++) {
		int r,g,b;
		im = gdImageCreate(100, 100);
		r = rand() % 255;
		g = rand() % 255;
		b = rand() % 255;

		gdImageColorAllocate(im, 255, 255, 255);  /* allocate white as side effect */
		black = gdImageColorAllocate(im,  r, g, b);
		printf("(%i, %i, %i)\n",r, g, b);
		gdImageFilledRectangle(im, rand() % 100, rand() % 100, rand() % 100, rand() % 100, black);
		gdImageGifAnimAdd(im, out, 1, 0, 0, 10, 1, prev);

		if(prev) {
			gdImageDestroy(prev);
		}
		prev = im;
	}

	gdImageGifAnimEnd(out);
	fclose(out);

	return 0;
}