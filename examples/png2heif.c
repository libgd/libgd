/**
 * A short program which converts a .png file into a .avif file -
 * just to get a little practice with the basic functionality.
 * We convert losslessly.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include "gd.h"

int main(int argc, char **argv)
{
	gdImagePtr im;
	FILE *in, *out;

	if (argc != 3) {
		fprintf(stderr, "Usage: png2avif infile.png outfile.avif\n");
		exit(1);
	}

	printf("Reading infile %s\n", argv[1]);

	in = fopen(argv[1], "rb");
	if (!in) {
		fprintf(stderr, "Error: input file %s does not exist.\n", argv[1]);
		exit(1);
	}

	im = gdImageCreateFromPng(in);
	fclose(in);
	if (!im) {
		fprintf(stderr, "Error: input file %s is not in PNG format.\n", argv[1]);
		exit(1);
	}
	gdImagePaletteToTrueColor(im);
	if (!gdImagePaletteToTrueColor(im)) {
		gdImageDestroy(im);
		exit(1);
	}

	out = fopen(argv[2], "wb");
	if (!out) {
		fprintf(stderr, "Error: can't write to output file %s\n", argv[2]);
		gdImageDestroy(im);
		exit(1);
	}

	fprintf(stderr, "Encoding...\n");

	gdImageHeif(im, out);

	printf("Wrote outfile %s.\n", argv[2]);

	fclose(out);
	gdImageDestroy(im);

	return 0;
}
