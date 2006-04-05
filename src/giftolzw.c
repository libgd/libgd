#include <stdio.h>
#include "gd.h"

/* A short program which converts a .gif file into a .gd file, for
	your convenience in creating images on the fly from a
	basis image that must be loaded quickly. The .gd format
	is not intended to be a general-purpose format. */

int main(int argc, char **argv)
{
	gdImagePtr im;
	FILE *in, *out;
	if (argc != 3) {
		fprintf(stderr, "Usage: giftogif filename.gif filename.gd\n");
		exit(1);
	}
	in = fopen(argv[1], "rb");
	if (!in) {
		fprintf(stderr, "Input file does not exist!\n");
		exit(1);
	}
	im = gdImageCreateFromGif(in);
	fclose(in);
	if (!im) {
		fprintf(stderr, "Input is not in GIF format!\n");
		exit(1);
	}

#ifdef LZW_LICENCED
	out = fopen(argv[2], "wb");
	if (!out) {
		fprintf(stderr, "Output file cannot be written to!\n");
		gdImageDestroy(im);
		exit(1);	
	}
	gdImageLzw(im, out);
	fclose(out);
#else
	printf("LZW Compression Not Licenced\n");
#endif

	gdImageDestroy(im);
}

