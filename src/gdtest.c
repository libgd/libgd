#include <stdio.h>
#include "gd.h"

/* A short program which converts a .gif file into a .gd file, for
	your convenience in creating images on the fly from a
	basis image that must be loaded quickly. The .gd format
	is not intended to be a general-purpose format. */

void CompareImages(char *msg, gdImagePtr im1, gdImagePtr im2);


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


	if (argc != 2) {
		fprintf(stderr, "Usage: gdtest filename.gif\n");
		exit(1);
	}
	in = fopen(argv[1], "rb");
	if (!in) {
		fprintf(stderr, "Input file does not exist!\n");
		exit(1);
	}
	im = gdImageCreateFromGif(in);

	rewind(in);
	ref = gdImageCreateFromGif(in);

	fclose(in);

	printf("Reference File has %d Palette entries\n",ref->colorsTotal);

	CompareImages("Initial Versions", ref, im);


        /* */
        /* Send to GIF File then Ptr */
        /* */
        sprintf(of, "%s.gif", argv[1]);
        out = fopen(of, "wb");
        gdImageGif(im, out);
        fclose(out);

        in = fopen(of, "rb");
        if (!in) {
                fprintf(stderr, "GIF Output file does not exist!\n");
                exit(1);
        }
        im2 = gdImageCreateFromGif(in);
        fclose(in);

        CompareImages("GD->GIF File->GD", ref, im2);

        unlink(of);
        gdImageDestroy(im2);

	iptr = gdImageGifPtr(im,&sz);
	ctx = gdNewDynamicCtx(sz,iptr);
	im2 = gdImageCreateFromGifCtx(ctx);

        CompareImages("GD->GIF ptr->GD", ref, im2);

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

#ifdef LZW_LICENCED
        /* */
        /* Send to LZW File then Ptr */
        /* */
        sprintf(of, "%s.lzw", argv[1]);
        out = fopen(of, "wb");
        gdImageLzw(im, out);
        fclose(out);

        in = fopen(of, "rb");
        if (!in) {
                fprintf(stderr, "LZW Output file does not exist!\n");
                exit(1);
        }
        im2 = gdImageCreateFromGif(in);
        fclose(in);

        CompareImages("GD->LZW File->GD", ref, im2);

        unlink(of);
        gdImageDestroy(im2);

        iptr = gdImageLzwPtr(im,&sz);
        /*printf("Got ptr %d (size %d)\n",iptr, sz); */
        ctx = gdNewDynamicCtx(sz,iptr);
        /*printf("Got ctx %d\n",ctx); */
        im2 = gdImageCreateFromGifCtx(ctx);
        /*printf("Got img2 %d\n",im2); */

        CompareImages("GD->LZW ptr->GD", ref, im2);

        gdImageDestroy(im2);
        ctx->free(ctx);
#endif

	/* */
	/*  Test Extraction */
	/* */
	in = fopen("test/gdtest_200_300_150_100.gif", "rb");
        if (!in) {
                fprintf(stderr, "gdtest_200_300_150_100.gif does not exist!\n");
                exit(1);
        }
        im2 = gdImageCreateFromGif(in);
        fclose(in);


        in = fopen("test/gdtest.gd2", "rb");
        if (!in) {
                fprintf(stderr, "gdtest.gd2 does not exist!\n");
                exit(1);
        }
        im3 = gdImageCreateFromGd2Part(in, 200, 300, 150, 100);
        fclose(in);

        CompareImages("GD2Part (gdtest.gd2, gdtest_200_300_150_100.gif)", im2, im3);

	gdImageDestroy(im2);
	gdImageDestroy(im3);

	/* */
	/*  Copy Blend */
	/* */
        in = fopen("test/gdtest.gif", "rb");
        if (!in) {
                fprintf(stderr, "gdtest.gif does not exist!\n");
                exit(1);
        }
        im2 = gdImageCreateFromGif(in);
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

        in = fopen("test/gdtest_merge.gif", "rb");
        if (!in) {
                fprintf(stderr, "gdtest.gd2 does not exist!\n");
                exit(1);
        }
        im3 = gdImageCreateFromGif(in);
        fclose(in);

	printf("[Merged Image has %d colours]\n",im2->colorsTotal);
	CompareImages("Merged (gdtest.gif, gdtest_merge.gif)", im2, im3);

        /*out = fopen("gdtest_merge.gif", "wb"); */
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
		printf("%s: image x-size differs\n",msg);
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
					printf("%s: image colours differ\n",msg);
					bad = (1==1);
					break;
				}

				if (im1->green[p1] != im2->green[p2]) {
					printf("%s: image colours differ\n",msg);
					bad = (1==1);
					break;
				}


				if (im1->blue[p1] != im2->blue[p2]) {
					printf("%s: image colours differ\n",msg);
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

