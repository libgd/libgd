#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* Bring in standard I/O and string manipulation functions */
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef __clang__
/* Workaround broken clang behavior: https://llvm.org/bugs/show_bug.cgi?id=20144 */
#undef strcmp
#endif

/* Bring in the gd library functions */
#include "gd.h"

#define KEEP_TRANS (-100)

static const char argv0[] = "webpng";

static void usage(const char *msg)
{
	/* If the command failed, output an explanation. */
	fprintf(msg == NULL ? stdout : stderr,
		"Usage: %s [-i y|n] [-l] [-t index|none] [-d] [-a] pngname.png\n"
		"  -i <y|n>   Turns on/off interlace\n"
		"  -l         Prints the table of color indexes\n"
		"  -t <index> Set the transparent color to the specified index (0-255 or \"none\")\n"
		"  -d         Reports the dimensions and other characteristics of the image\n"
		"  -a         Prints all alpha channels that are not 100%% opaque\n"
		"\n"
		"If you specify '-' as the input file, stdin/stdout will be used as input/output.\n",
		argv0);
	if (msg)
		fprintf(stderr, "\nError: %s\n", msg);
	exit(msg == NULL ? 0 : 1);
}

static void err(const char *fmt, ...)
{
	va_list ap;
	int e = errno;

	fprintf(stderr, "%s: error: ", argv0);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	if (e)
		fprintf(stderr, ": %s", strerror(e));
	fputs("\n", stderr);

	exit(1);
}

int
main(int argc, char **argv)
{
	FILE *in;
	FILE *out;
	const char *infile;
	char *tmpfile;
	int i;
	int use_stdin_stdout = 0;

	int interlace = -100;
	int list_color_table = 0;
	int trans_col = KEEP_TRANS;
	int report_details = 0;
	int print_alpha = 0;

	/* Declare our image pointer */
	gdImagePtr im = 0;
	/* We'll set 'write' once we know the user's request
	   requires that the image be written back to disk. */
	int write = 0;
	int got_a_flag = 0;

	/* Consider each argument in turn. */
	opterr = 0;
	while ((i = getopt(argc, argv, "i:lt:da")) != -1) {
		got_a_flag = 1;
		switch (i) {
		case 'i':
			/* -i turns on and off interlacing. */
			if (strcmp(optarg, "y") == 0)
				interlace = 1;
			else if (strcmp(optarg, "n") == 0)
				interlace = 0;
			else
				usage("-i specified without y or n");
			write = 1;
			break;

		case 'l':
			/* List the colors in the color table. */
			list_color_table = 1;
			break;

		case 't':
			/* Set transparent index (or none). */
			if (strcmp(optarg, "none") == 0) {
				/* -1 means not transparent. */
				trans_col = -1;
			} else {
				/* XXX: Should check for errors. */
				trans_col = atoi(optarg);
				if (trans_col < 0 || trans_col > 255)
					err("-t has to be in the range of 0 and 255 (inclusive)");
			}
			write = 1;
			break;

		case 'd':
			/* Output dimensions, etc. */
			report_details = 1;
			break;

		case 'a':
			/* Alpha channel info -- thanks to Wez Furlong */
			print_alpha = 1;
			break;

		case 'h':
			usage(NULL);
			break;
		default:
		case '?':
			if (optind < argc && strcmp(argv[optind], "--help") == 0)
				usage(NULL);
			usage("unknown option");
			break;
		}
	}

	if (got_a_flag == 0)
		usage("missing operation flag");

	if (argc == optind)
		usage("missing filename");
	else if (argc != optind + 1)
		usage("can only specify one file");

	infile = argv[optind];
	if (strcmp(infile, "-") == 0) {
		/* - is synonymous with STDIN */
		use_stdin_stdout = 1;
		in = stdin;
	} else
		in = fopen(infile, "rb");

	if (!in)
		err("can't open file %s", infile);

	/* Now load the image. */
	im = gdImageCreateFromPng(in);
	fclose(in);
	/* If the load failed, it must not be a PNG file. */
	if (!im)
		err("%s is not a valid PNG file", infile);

	if (list_color_table) {
		/* List the colors in the color table. */
		if (!im->trueColor) {
			int j;
			/* Tabs used below. */
			printf("Index	Red	Green	Blue Alpha\n");
			for (j = 0; j < gdImageColorsTotal(im); ++j) {
				/* Use access macros to learn colors. */
				printf("%d	%d	%d	%d	%d\n", j,
					gdImageRed(im, j),
					gdImageGreen(im, j),
					gdImageBlue(im, j),
					gdImageAlpha(im, j));
			}
		} else
			printf("Truecolor image, no palette entries to list.\n");
	}

	if (report_details) {
		/* Output dimensions, etc. */
		int t;
		printf("Width: %d Height: %d Colors: %d\n",
			gdImageSX(im),
			gdImageSY(im),
			gdImageColorsTotal(im));

		/* -1 means the image is not transparent. */
		t = gdImageGetTransparent(im);
		if (t != -1)
			printf("First 100%% transparent index: %d\n", t);
		else
			printf("First 100%% transparent index: none\n");

		if (gdImageGetInterlaced(im))
			printf("Interlaced: yes\n");
		else
			printf("Interlaced: no\n");
	}

	if (print_alpha) {
		/* Alpha channel info -- thanks to Wez Furlong */
		int maxx, maxy, x, y, alpha, pix, nalpha = 0;

		maxx = gdImageSX(im);
		maxy = gdImageSY(im);

		printf("alpha channel information:\n");

		if (im->trueColor) {
			for (y = 0; y < maxy; y++) {
				for (x = 0; x < maxx; x++) {
					pix = gdImageGetPixel(im, x, y);
					alpha = gdTrueColorGetAlpha(pix);

					if (alpha > gdAlphaOpaque) {
						/* Use access macros to learn colors. */
						printf("%d	%d	%d	%d\n",
							gdTrueColorGetRed(pix),
							gdTrueColorGetGreen(pix),
							gdTrueColorGetBlue(pix),
							alpha);
						nalpha++;
					}

				}
			}
		} else
			printf("NOT a true color image\n");

		printf("%d alpha channels\n", nalpha);
	}

	/* If no modifications requested, break out. */
	if (write == 0) {
		gdImageDestroy(im);
		return 0;
	}

	if (interlace == 1)
		gdImageInterlace(im, 1);
	else if (interlace == 0)
		gdImageInterlace(im, 0);

	if (trans_col != KEEP_TRANS)
		gdImageColorTransparent(im, trans_col);

	if (use_stdin_stdout) {
		out = stdout;
	} else {
		/* Open a temporary file. */
		size_t filelen = strlen(infile);
		size_t len = filelen + 8;
		int outfd;

		tmpfile = malloc(len);
		if (tmpfile == NULL)
			err("could not create a tempfile");
		memcpy(tmpfile, infile, filelen);
		strcpy(tmpfile + filelen, ".XXXXXX");

		outfd = mkstemp(tmpfile);
		if (outfd == -1)
			err("could not open %s", tmpfile);

		out = fdopen(outfd, "wb");
		if (!out)
			err("could not open %s", tmpfile);
	}

	/* Write the new PNG. */
	gdImagePng(im, out);

	if (!use_stdin_stdout) {
		fclose(out);
		/* Erase the old PNG. */
		unlink(infile);
		/* Rename the new to the old. */
		if (rename(tmpfile, infile) != 0)
			err("unable to rename %s to %s", infile, tmpfile);
	}

	/* Delete the image from memory. */
	gdImageDestroy(im);

	/* All's well that ends well. */
	return 0;
}
