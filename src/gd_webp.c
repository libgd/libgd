#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */


#ifdef HAVE_LIBWEBP
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "gd_errors.h"
#include "gdhelpers.h"
#include "webp/decode.h"
#include "webp/encode.h"

#define GD_WEBP_ALLOC_STEP (4*1024)

gdImagePtr gdImageCreateFromWebp (FILE * inFile)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx(inFile);
	if (!in) {
		return 0;
	}
	im = gdImageCreateFromWebpCtx(in);
	in->gd_free(in);

	return im;
}


gdImagePtr gdImageCreateFromWebpPtr (int size, void *data)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	if (!in)
		return 0;
	im = gdImageCreateFromWebpCtx(in);
	in->gd_free(in);
	return im;
}

gdImagePtr gdImageCreateFromWebpCtx (gdIOCtx * infile)
{
	int    width, height;
	uint8_t   *filedata = NULL;
	uint8_t    *argb = NULL;
	unsigned char   *read, *temp;
	size_t size = 0, n;
	gdImagePtr im;
	int x, y;
	uint8_t *p;

	do {
		temp = gdRealloc(filedata, size+GD_WEBP_ALLOC_STEP);
		if (temp) {
			filedata = temp;
			read = temp + size;
		} else {
			if (filedata) {
				gdFree(filedata);
			}
			gd_error("WebP decode: realloc failed");
			return NULL;
		}

		n = gdGetBuf(read, GD_WEBP_ALLOC_STEP, infile);
		if (n>0 && n!=EOF) {
			size += n;
		}
	} while (n>0 && n!=EOF);

	if (WebPGetInfo(filedata,size, &width, &height) == 0) {
		gd_error("gd-webp cannot get webp info");
		gdFree(temp);
		return NULL;
	}

	im = gdImageCreateTrueColor(width, height);
	if (!im) {
		gdFree(temp);
		return NULL;
	}
	argb = WebPDecodeARGB(filedata, size, &width, &height);
	if (!argb) {
		gd_error("gd-webp cannot allocate temporary buffer");
		gdFree(temp);
		gdImageDestroy(im);
		return NULL;
	}
	for (y = 0, p = argb;  y < height; y++) {
		for (x = 0; x < width; x++) {
			register uint8_t a = gdAlphaMax - (*(p++) >> 1);
			register uint8_t r = *(p++);
			register uint8_t g = *(p++);
			register uint8_t b = *(p++);
			im->tpixels[y][x] = gdTrueColorAlpha(r, g, b, a);
		}
	}
	/* do not use gdFree here, in case gdFree/alloc is mapped to something else than libc */
	free(argb);
	gdFree(temp);
	im->saveAlphaFlag = 1;
	return im;
}

void gdImageWebpCtx (gdImagePtr im, gdIOCtx * outfile, int quantization)
{
	uint8_t *argb;
	int x, y;
	uint8_t *p;
	uint8_t *out;
	size_t out_size;

	if (im == NULL) {
		return;
	}

	if (!gdImageTrueColor(im)) {
		gd_error("Paletter image not supported by webp");
		return;
	}

	if (quantization == -1) {
		quantization = 80;
	}

	argb = (uint8_t *)gdMalloc(gdImageSX(im) * 4 * gdImageSY(im));
	if (!argb) {
		return;
	}
	p = argb;
	for (y = 0; y < gdImageSY(im); y++) {
		for (x = 0; x < gdImageSX(im); x++) {
			register int c;
			register char a;
			c = im->tpixels[y][x];
			a = gdTrueColorGetAlpha(c);
			if (a == 127) {
				a = 0;
			} else {
				a = 255 - ((a << 1) + (a >> 6));
			}
			*(p++) = gdTrueColorGetRed(c);
			*(p++) = gdTrueColorGetGreen(c);
			*(p++) = gdTrueColorGetBlue(c); 
			*(p++) = a;
		}
	}
	out_size = WebPEncodeRGBA(argb, gdImageSX(im), gdImageSY(im), gdImageSX(im) * 4, quantization, &out);
	if (out_size == 0) {
		gd_error("gd-webp encoding failed");
		goto freeargb;
	}
	gdPutBuf(out, out_size, outfile);
	free(out);

freeargb:
	gdFree(argb);
}

/*
	Function: gdImageWebpEx
*/
BGD_DECLARE(void) gdImageWebpEx (gdImagePtr im, FILE * outFile, int quantization)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) {
		return;
	}
	gdImageWebpCtx(im, out, quantization);
	out->gd_free(out);
}

/*
	Function: gdImageWebp
*/
BGD_DECLARE(void) gdImageWebp (gdImagePtr im, FILE * outFile)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) {
		return;
	}
	gdImageWebpCtx(im, out, -1);
	out->gd_free(out);
}

/*
	Function: gdImageWebpPtr
*/
BGD_DECLARE(void *) gdImageWebpPtr (gdImagePtr im, int *size)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) {
		return NULL;
	}
	gdImageWebpCtx(im, out, -1);
	rv = gdDPExtractData(out, size);
	out->gd_free(out);

	return rv;
}

/*
	Function: gdImageWebpPtrEx
*/
BGD_DECLARE(void *) gdImageWebpPtrEx (gdImagePtr im, int *size, int quantization)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) {
		return NULL;
	}
	gdImageWebpCtx(im, out, quantization);
	rv = gdDPExtractData(out, size);
	out->gd_free(out);
	return rv;
}
#endif /* HAVE_LIBWEBP */
