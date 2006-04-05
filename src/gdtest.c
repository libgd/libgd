#include <stdio.h>
#include "gd.h"

/* A short program which converts a .png file into a .gd file, for
	your convenience in creating images on the fly from a
	basis image that must be loaded quickly. The .gd format
	is not intended to be a general-purpose format. */

void CompareImages(char *msg, gdImagePtr im1, gdImagePtr im2);

static int freadWrapper(void *context, char *buf, int len);
static int fwriteWrapper(void *context, const char *buffer, int len);

int main(int argc, char **argv)
{
	gdImagePtr 	im, ref, im2, im3;
	FILE 		*in, *out;
	int		cs, fmt;
	void		*iptr;
	int		sz;
	gdIOCtxPtr	ctx;
	char		of[256];
	int		colRed, colBlu;
	int		idx;
	gdSource	imgsrc;
	gdSink		imgsnk;

	if (argc != 2) {
		fprintf(stderr, "Usage: gdtest filename.png\n");
		exit(1);
	}
	in = fopen(argv[1], "rb");
	if (!in) {
		fprintf(stderr, "Input file does not exist!\n");
		exit(1);
	}
	im = gdImageCreateFromPng(in);

	rewind(in);
	ref = gdImageCreateFromPng(in);

	fclose(in);

	printf("Reference File has %d Palette entries\n",ref->colorsTotal);

	CompareImages("Initial Versions", ref, im);


        /* */
        /* Send to PNG File then Ptr */
        /* */
        sprintf(of, "%s.png", argv[1]);
        out = fopen(of, "wb");
        gdImagePng(im, out);
        fclose(out);

        in = fopen(of, "rb");
        if (!in) {
                fprintf(stderr, "PNG Output file does not exist!\n");
                exit(1);
        }
        im2 = gdImageCreateFromPng(in);
        fclose(in);

        CompareImages("GD->PNG File->GD", ref, im2);

        unlink(of);
        gdImageDestroy(im2);

	iptr = gdImagePngPtr(im,&sz);
	ctx = gdNewDynamicCtx(sz,iptr);
	im2 = gdImageCreateFromPngCtx(ctx);

        CompareImages("GD->PNG ptr->GD", ref, im2);

	gdImageDestroy(im2);
	ctx->free(ctx);


	/* */
	/* Send to GD2 File then Ptr */
	/* */
	sprintf(of, "%s.gd2", argv[1]);
	out = fopen(of, "wb");
	gdImageGd2(im, out, 128, 2);
	fclose(out);

	in = fopen(of, "rb");
        if (!in) {
                fprintf(stderr, "GD2 Output file does not exist!\n");
                exit(1);
        }
	im2 = gdImageCreateFromGd2(in);
	fclose(in);

	CompareImages("GD->GD2 File->GD", ref, im2);

	unlink(of);
	gdImageDestroy(im2);

	iptr = gdImageGd2Ptr(im,128,2,&sz);
	/*printf("Got ptr %d (size %d)\n",iptr, sz); */
	ctx = gdNewDynamicCtx(sz,iptr);
	/*printf("Got ctx %d\n",ctx); */
	im2 = gdImageCreateFromGd2Ctx(ctx);
	/*printf("Got img2 %d\n",im2); */

	CompareImages("GD->GD2 ptr->GD", ref, im2);

	gdImageDestroy(im2);
	ctx->free(ctx);


        /* */
        /* Send to GD File then Ptr */
        /* */
        sprintf(of, "%s.gd", argv[1]);
        out = fopen(of, "wb");
        gdImageGd(im, out);
        fclose(out);

        in = fopen(of, "rb");
        if (!in) {
                fprintf(stderr, "GD Output file does not exist!\n");
                exit(1);
        }
        im2 = gdImageCreateFromGd(in);
        fclose(in);

        CompareImages("GD->GD File->GD", ref, im2);

        unlink(of);
        gdImageDestroy(im2);

        iptr = gdImageGdPtr(im,&sz);
        /*printf("Got ptr %d (size %d)\n",iptr, sz); */
        ctx = gdNewDynamicCtx(sz,iptr);
        /*printf("Got ctx %d\n",ctx); */
        im2 = gdImageCreateFromGdCtx(ctx);
        /*printf("Got img2 %d\n",im2); */

        CompareImages("GD->GD ptr->GD", ref, im2);

        gdImageDestroy(im2);
        ctx->free(ctx);

	/*
	** Test gdImageCreateFromPngSource'
	**/

	in = fopen(argv[1], "rb");

	imgsrc.source = freadWrapper;
	imgsrc.context = in;
	im2 = gdImageCreateFromPngSource(&imgsrc);
	fclose(in);

	if (im2 == NULL) {
		printf("GD Source: ERROR Null returned by gdImageCreateFromPngSource\n");
	} else {
		CompareImages("GD Source", ref, im2);
		gdImageDestroy(im2);
	};


	/*
	** Test gdImagePngToSink'
	**/

        sprintf(of, "%s.snk", argv[1]);
        out = fopen(of, "wb");
 	imgsnk.sink = fwriteWrapper;
	imgsnk.context = out;
	gdImagePngToSink(im, &imgsnk);
        fclose(out);
        in = fopen(of, "rb");
        if (!in) {
                fprintf(stderr, "GD Sink: ERROR - GD Sink Output file does not exist!\n");
        } else {
        	im2 = gdImageCreateFromPng(in);
		fclose(in);

        	CompareImages("GD Sink", ref, im2);
		gdImageDestroy(im2);
	};

	unlink(of);

	/* */
	/*  Test Extraction */
	/* */
	in = fopen("test/gdtest_200_300_150_100.png", "rb");
        if (!in) {
                fprintf(stderr, "gdtest_200_300_150_100.png does not exist!\n");
                exit(1);
        }
        im2 = gdImageCreateFromPng(in);
        fclose(in);


        in = fopen("test/gdtest.gd2", "rb");
        if (!in) {
                fprintf(stderr, "gdtest.gd2 does not exist!\n");
                exit(1);
        }
        im3 = gdImageCreateFromGd2Part(in, 200, 300, 150, 100);
        fclose(in);

        CompareImages("GD2Part (gdtest.gd2, gdtest_200_300_150_100.png)", im2, im3);

	gdImageDestroy(im2);
	gdImageDestroy(im3);

	/* */
	/*  Copy Blend */
	/* */
        in = fopen("test/gdtest.png", "rb");
        if (!in) {
                fprintf(stderr, "gdtest.png does not exist!\n");
                exit(1);
        }
        im2 = gdImageCreateFromPng(in);
        fclose(in);

	im3 = gdImageCreate(100, 60);
	colRed = gdImageColorAllocate(im3, 255, 0, 0);
	colBlu = gdImageColorAllocate(im3, 0, 0, 255);
	gdImageFilledRectangle(im3, 0, 0, 49, 30, colRed);
        gdImageFilledRectangle(im3, 50, 30, 99, 59, colBlu);

	gdImageCopyMerge(im2, im3, 150, 200, 10, 10, 90, 50, 50);
        gdImageCopyMerge(im2, im3, 180, 70, 10, 10, 90, 50, 50);

	gdImageCopyMergeGray(im2, im3, 250, 160, 10, 10, 90, 50, 50);
        gdImageCopyMergeGray(im2, im3, 80, 70, 10, 10, 90, 50, 50);

	gdImageDestroy(im3);

        in = fopen("test/gdtest_merge.png", "rb");
        if (!in) {
                fprintf(stderr, "gdtest.gd2 does not exist!\n");
                exit(1);
        }
        im3 = gdImageCreateFromPng(in);
        fclose(in);

	printf("[Merged Image has %d colours]\n",im2->colorsTotal);
	CompareImages("Merged (gdtest.png, gdtest_merge.png)", im2, im3);

        /*out = fopen("gdtest_merge.png", "wb"); */
        /*gdImageLzw(im2, out); */
        /*fclose(out); */

	gdImageDestroy(im2);
        gdImageDestroy(im3);

	gdImageDestroy(im);
	gdImageDestroy(ref);

}

void CompareImages(char *msg, gdImagePtr im1, gdImagePtr im2)
{
	int	x, y;
	int	bad;
	int	p1, p2;

	bad = (1==0);
	if (im1->sx != im2->sx) {
		printf("%s: ERROR image x-size differs\n",msg);
		bad = (1==1);
	}

	if (im1->sy != im2->sy) {
		printf("%s: image y-size differs\n",msg);
		bad = (1==1);
	}

	if (!bad) {
		if (im1->colorsTotal != im2->colorsTotal) {
			printf("Mismatch in number of colours: %d Vs. %d\n",im1->colorsTotal, im2->colorsTotal);
		}
		for ( y = 0 ; (y<im1->sy) ; y++ ) {
			for ( x = 0 ; (x < im1->sx) ; x++ ) {
				p1 = im1->pixels[y][x];
				p2 = im2->pixels[y][x];

				if (im1->red[p1] != im2->red[p2]) {
					printf("%s: ERROR image colours differ\n",msg);
					bad = (1==1);
					break;
				}

				if (im1->green[p1] != im2->green[p2]) {
					printf("%s: ERROR image colours differ\n",msg);
					bad = (1==1);
					break;
				}


				if (im1->blue[p1] != im2->blue[p2]) {
					printf("%s: ERROR image colours differ\n",msg);
					bad = (1==1);
					break;
				}

			}
			if (bad) { break; };
		}
	}

	if (!bad) {
		printf("%s: OK\n",msg);
	}
}


static int freadWrapper(void *context, char *buf, int len)
{
	int got = fread(buf, 1, len, (FILE *) context);
	return got;
}

static int fwriteWrapper(void *context, const char *buffer, int len)
{
        return fwrite(buffer, 1, len, (FILE *) context);
}

