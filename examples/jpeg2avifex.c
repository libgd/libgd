/**
 * A short program which converts a .jpg file into a .avif file -
 * just to get a little practice with the basic functionality.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "gd.h"

static void usage() {
	fprintf(stderr, "Usage: jpeg2avifex [-q quality] [-s speed] infile.jpg outfile.avif\n");
	exit(1);
}

int main(int argc, char **argv)
{
	gdImagePtr im;
	FILE *in, *out;
	int c;
	int speed = -1, quality = -1; // use default values if unspecified
	char *infile, *outfile;
	int failed = 0;

	if (argc < 3) {
		usage();
	}

	while ((c = getopt(argc, argv, "q:s:")) != -1) {
		switch (c) {
			case 'q':
				quality = atoi(optarg);
				break;

			case 's':
				speed = atoi(optarg);
				break;

			default:
				usage();
		}
	}

	if (optind > argc - 2)
		usage();

	infile = strdup(argv[optind++]);
	outfile = strdup(argv[optind]);

	printf("Reading infile %s\n", infile);

	in = fopen(infile, "rb");
	if (!in) {
		fprintf(stderr, "\nError: input file %s does not exist.\n", infile);
		failed = 1;
		goto cleanup;
	}

	im = gdImageCreateFromJpeg(in);
	fclose(in);
	if (!im) {
		fprintf(stderr, "\nError: input file %s is not in JPEG format.\n", infile);
		failed = 1;
		goto cleanup;
	}

	out = fopen(outfile, "wb");
	if (!out) {
		fprintf(stderr, "\nError: can't write to output file %s\n", outfile);
		failed = 1;
		goto cleanup;
	}

	fprintf(stderr, "Encoding...\n");

	gdImageAvifEx(im, out, quality, speed);

	printf("Wrote outfile %s.\n", outfile);

	fclose(out);

cleanup:
	if (im)
		gdImageDestroy(im);

	gdFree(infile);
	gdFree(outfile);

	exit(failed);
}
