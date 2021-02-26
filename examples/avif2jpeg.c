/**
 * A short program which converts a .avif file into a .jpg file -
 * just to get a little practice with the basic functionality.
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
		fprintf(stderr, "Usage: avif2jpeg infile.avif outfile.jpg\n");
		exit(1);
	}

	printf("Reading infile %s\n", argv[1]);

	in = fopen(argv[1], "rb");
	if (!in) {
		fprintf(stderr, "\nError: input file %s does not exist.\n", argv[1]);
		exit(1);
	}

	im = gdImageCreateFromAvif(in);
	fclose(in);
	if (!im) {
		fprintf(stderr, "\nError: input file %s is not in AVIF format.\n", argv[1]);
		exit(1);
	}

	out = fopen(argv[2], "wb");
	if (!out) {
		fprintf(stderr, "\nError: can't write to output file %s\n", argv[2]);
		gdImageDestroy(im);
		exit(1);
	}

	gdImageJpeg(im, out, 75);

	printf("Wrote outfile %s.\n", argv[2]);

	fclose(out);
	gdImageDestroy(im);

	return 0;
}
