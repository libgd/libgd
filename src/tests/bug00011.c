/* $Id$ */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
 	gdImagePtr im;
	FILE *fp;

	fp = fopen("emptyfile", "rb");
	if (!fp) {
		printf("failed, cannot open file\n");
	}
	im = gdImageCreateFromPng(fp);
	fclose(fp);
	if (!im) {
		printf("ok\n");
		/* Success */
		exit(0);
	}		
	printf("failed\n");
 	/* Test fails */
 	gdImageDestroy(im);

	exit(1);
}
