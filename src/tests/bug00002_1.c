#include <gd.h>
#include <stdio.h>

int main()
{
	gdImagePtr im;
	FILE *fp;

	fputs("flag 0\n", stdout);
	im = gdImageCreateTrueColor(100, 100);

	fputs("flag 1\n", stdout);
	gdImageFill(im, 0, 0, 0xffffff);
	fputs("flag 2\n", stdout);
	gdImageFill(im, 0, 0, 0xffffff);
	fputs("flag 3\n", stdout);

	fp = fopen("a.png", "wb");
 	/* Write img to stdout */
 	gdImagePng(im,fp);
	fclose(fp);

 	/* Destroy it */
 	gdImageDestroy(im);
	return 0;
}

