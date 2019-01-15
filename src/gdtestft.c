#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include <string.h>
#include <stdlib.h>

#define PI 3.141592
#define DEG2RAD(x) ((x)*PI/180.)

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

#define MAX4(x,y,z,w) \
	((MAX((x),(y))) > (MAX((z),(w))) ? (MAX((x),(y))) : (MAX((z),(w))))
#define MIN4(x,y,z,w) \
	((MIN((x),(y))) < (MIN((z),(w))) ? (MIN((x),(y))) : (MIN((z),(w))))

#define MAXX(x) MAX4(x[0],x[2],x[4],x[6])
#define MINX(x) MIN4(x[0],x[2],x[4],x[6])
#define MAXY(x) MAX4(x[1],x[3],x[5],x[7])
#define MINY(x) MIN4(x[1],x[3],x[5],x[7])

int
main (int argc, char *argv[])
{
	gdImagePtr im;
	int blue;
	int blueAlpha;
	int white;
	int brect[8];
	int x, y, sx, sy;
	char *err;
#ifdef JISX0208
	char *s = "Hello. ‚±‚ñ‚É‚¿‚Í Qyjpqg,";	/* String to draw. */
#else
	char *s = "Hello. ã“ã‚“ã«ã¡ã¯ Qyjpqg,";	/* String to draw. */
#endif

	double sz = 40.;

#if 0
	double angle = 0.;
#else
	double angle = DEG2RAD (90);
#endif
	char *f;
	if (argc == 2) {
		f = argv[1];
	} else {
		/* 2.02: usage message. Defaulting to Times wasn't working well for the
		   many people with no /usr/share/fonts/truetype. */
		fprintf(stderr, "Usage: gdtestft fontfilename\n"
			 "If fontfilename is not a full or relative path, GDFONTPATH is searched for\n"
		         "it. If GDFONTPATH is not set, /usr/share/fonts/truetype is searched.\n");
		return 1;
	}
	/* obtain brect so that we can size the image */
	err =
	    gdImageStringFT ((gdImagePtr) NULL, &brect[0], 0, f, sz, angle, 0, 0, s);
	if (err) {
		fprintf(stderr, "%s\n", err);
		return 1;
	}

	/* create an image just big enough for the string (x3) */
	sx = MAXX (brect) - MINX (brect) + 6;
	sy = MAXY (brect) - MINY (brect) + 6;
#if 0
	/* Would be palette color 8-bit (which of course is still allowed,
	   but not impressive when used with a JPEG background and antialiasing
	   and alpha channel and so on!) */
	im = gdImageCreate (sx * 3, sy);
#else
	/* gd 2.0: true color images can use freetype too,
	   and they can do antialiasing against arbitrary
	   complex backgrounds. */
	im = gdImageCreateTrueColor (sx * 3, sy);
#endif
	/* Background color. gd 2.0: fill the image with it; truecolor
	   images have a black background otherwise. */
	white = gdImageColorResolve (im, 255, 255, 255);
	/* Load a pretty background and resample it to cover the entire image */
	{
		FILE *in = fopen ("eleanor.jpg", "rb");
		gdImagePtr imb = NULL;
		if (in) {
#ifdef HAVE_LIBJPEG
			imb = gdImageCreateFromJpeg (in);
#else
			fprintf(stderr, "No JPEG library support.\n");
#endif
			fclose(in);

			if (!imb) {
				fprintf(stderr, "gdImageCreateFromJpeg failed\n");
				return 1;
			}
			if (!im->trueColor) {
				/* If destination is not truecolor, convert the JPEG to a
				   reasonably high-quality palette version. This is not as good
				   as creating a truecolor output file, of course. Leave many
				   colors for text smoothing. */
#if 1
				gdImageTrueColorToPalette (imb, 0, 128);
#endif
			}
			/* Resample background image to cover new image exactly */
			gdImageCopyResampled (im, imb, 0, 0, 0, 0, sx * 3, sy,
			                      gdImageSX (imb), gdImageSY (imb));
		} else {
			/* Can't get background, so paint a simple one */
			/* Truecolor images start out black, so paint it white */
			gdImageFilledRectangle (im, 0, 0, sx * 3, sy, white);
		}
	}
	/* TBB 2.0.2: only black was working, and I didn't know it because
	   the test program used black. Funny, huh? Let's do a more interesting
	   color this time.  */
	blue = gdImageColorResolve (im, 128, 192, 255);
	/* Almost-transparent blue (alpha blending), with antialiasing */
	blueAlpha = gdImageColorResolveAlpha (im, 128, 192, 255, gdAlphaMax / 2);
	/* render the string, offset origin to center string */
	x = 0 - MINX (brect) + 3;
	y = 0 - MINY (brect) + 3;

	/* With antialiasing (positive color value) */
	err = gdImageStringFT (im, NULL, blue, f, sz, angle, x, y, s);
	if (err) {
		fprintf(stderr, "%s\n", err);
		return 1;
	}
	/* Without antialiasing (negative color value) */
	err = gdImageStringFT (im, NULL, -blue, f, sz, angle, sx + x, y, s);
	if (err) {
		fprintf(stderr, "%s\n", err);
		return 1;
	}
	/* With antialiasing, and 50% alpha blending (truecolor only) */
	err = gdImageStringFT (im, NULL, blueAlpha, f, sz, angle, sx * 2 + x, y, s);
	if (err) {
		fprintf(stderr, "%s\n", err);
		return 1;
	}
	/* TBB: Write img to test/fttest.jpg or test/fttest.png */
	if (im->trueColor) {
#ifdef HAVE_LIBJPEG
		FILE *out = fopen ("test/fttest.jpg", "wb");
		if (!out) {
			fprintf(stderr, "Can't create test/fttest.jpg\n");
			exit (1);
		}
		/* Fairly high JPEG quality setting */
		gdImageJpeg (im, out, 90);
		fclose (out);
		fprintf(stderr, "Test image written to test/fttest.jpg\n");
#else
		fprintf(stderr, "Test image not written; No JPEG library support.\n");
#endif
	} else {
#ifdef HAVE_LIBPNG
		FILE *out = fopen ("test/fttest.png", "wb");
		if (!out) {
			fprintf(stderr, "Can't create test/fttest.png\n");
			exit (1);
		}
		/* 2.0.10: correct ifdef, thanks to Gabriele Verzeletti */
		gdImagePng (im, out);
		fclose (out);
		fprintf(stderr, "Test image written to test/fttest.png\n");
#else
		fprintf(stderr, "Test image not written; No PNG library support.\n");
#endif
	}
	/* Destroy it */
	gdImageDestroy (im);

	return 0;
}
