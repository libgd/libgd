/**
 * File: WebP IO
 *
 * Read and write WebP images.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "gd_errors.h"
#include "gdhelpers.h"

#ifdef HAVE_LIBWEBP
#include "webp/decode.h"
#include "webp/encode.h"

#define GD_WEBP_ALLOC_STEP (4*1024)

/*
  Function: gdImageCreateFromWebp

    <gdImageCreateFromWebp> is called to load truecolor images from
    WebP format files. Invoke <gdImageCreateFromWebp> with an
    already opened pointer to a file containing the desired
    image. <gdImageCreateFromWebp> returns a <gdImagePtr> to the new
    truecolor image, or NULL if unable to load the image (most often
    because the file is corrupt or does not contain a WebP
    image). <gdImageCreateFromWebp> does not close the file.

    You can inspect the sx and sy members of the image to determine
    its size. The image must eventually be destroyed using
    <gdImageDestroy>.

    *The returned image is always a truecolor image.*

  Variants:

    <gdImageCreateFromJpegPtr> creates an image from WebP data
    already in memory.

    <gdImageCreateFromJpegCtx> reads its data via the function
    pointers in a <gdIOCtx> structure.

  Parameters:

    infile - The input FILE pointer.

  Returns:

    A pointer to the new *truecolor* image.  This will need to be
    destroyed with <gdImageDestroy> once it is no longer needed.

    On error, returns NULL.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebp (FILE * inFile)
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


/*
  Function: gdImageCreateFromWebpPtr

    See <gdImageCreateFromWebp>.

  Parameters:

    size            - size of WebP data in bytes.
    data            - pointer to WebP data.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpPtr (int size, void *data)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	if (!in)
		return 0;
	im = gdImageCreateFromWebpCtx(in);
	in->gd_free(in);
	return im;
}

/*
  Function: gdImageCreateFromWebpCtx

    See <gdImageCreateFromWebp>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpCtx (gdIOCtx * infile)
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


/* returns 0 on success, 1 on failure */
static int _gdImageWebpCtx (gdImagePtr im, gdIOCtx * outfile, int quality)
{
	uint8_t *argb;
	int x, y;
	uint8_t *p;
	uint8_t *out;
	size_t out_size;
    int ret = 0;

	if (im == NULL) {
		return 1;
	}

	if (!gdImageTrueColor(im)) {
		gd_error("Palette image not supported by webp");
		return 1;
	}

	if (quality == -1) {
		quality = 80;
	}

	if (overflow2(gdImageSX(im), 4)) {
		return 1;
	}

	if (overflow2(gdImageSX(im) * 4, gdImageSY(im))) {
		return 1;
	}

	argb = (uint8_t *)gdMalloc(gdImageSX(im) * 4 * gdImageSY(im));
	if (!argb) {
		return 1;
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
	out_size = WebPEncodeRGBA(argb, gdImageSX(im), gdImageSY(im), gdImageSX(im) * 4, quality, &out);
	if (out_size == 0) {
		gd_error("gd-webp encoding failed");
        ret = 1;
		goto freeargb;
	}
	gdPutBuf(out, out_size, outfile);
	free(out);

freeargb:
	gdFree(argb);

    return ret;
}


/*
  Function: gdImageWebpCtx

    Write the image as WebP data via a <gdIOCtx>. See <gdImageWebpEx>
    for more details.

  Parameters:

    im      - The image to write.
    outfile - The output sink.
    quality - Image quality.

  Returns:

    Nothing.
*/
BGD_DECLARE(void) gdImageWebpCtx (gdImagePtr im, gdIOCtx * outfile, int quality)
{
	_gdImageWebpCtx(im, outfile, quality);
}

/*
  Function: gdImageWebpEx

    <gdImageWebpEx> outputs the specified image to the specified file in
    WebP format. The file must be open for writing. Under MSDOS and
    all versions of Windows, it is important to use "wb" as opposed to
    simply "w" as the mode when opening the file, and under Unix there
    is no penalty for doing so. <gdImageWebpEx> does not close the file;
    your code must do so.

	If _quality_ is -1, a reasonable quality value (which should yield a
	good general quality / size tradeoff for most situations) is used. Otherwise
	_quality_ should be a value in the range 0-100, higher quality values
	usually implying both higher quality and larger image sizes.

  Variants:

    <gdImageWebpCtx> stores the image using a <gdIOCtx> struct.

    <gdImageWebpPtrEx> stores the image to RAM.

  Parameters:

    im      - The image to save.
    outFile - The FILE pointer to write to.
    quality - Compression quality (0-100).

  Returns:

    Nothing.
*/
BGD_DECLARE(void) gdImageWebpEx (gdImagePtr im, FILE * outFile, int quality)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) {
		return;
	}
	_gdImageWebpCtx(im, out, quality);
	out->gd_free(out);
}

/*
  Function: gdImageWebp

    Variant of <gdImageWebpEx> which uses the default quality (-1).

  Parameters:

    im      - The image to save
    outFile - The FILE pointer to write to.

  Returns:

    Nothing.
*/
BGD_DECLARE(void) gdImageWebp (gdImagePtr im, FILE * outFile)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) {
		return;
	}
	_gdImageWebpCtx(im, out, -1);
	out->gd_free(out);
}

/*
  Function: gdImageWebpPtr

    See <gdImageWebpEx>.
*/
BGD_DECLARE(void *) gdImageWebpPtr (gdImagePtr im, int *size)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) {
		return NULL;
	}
	if (_gdImageWebpCtx(im, out, -1)) {
		rv = NULL;
	} else {
		rv = gdDPExtractData(out, size);
	}
	out->gd_free(out);

	return rv;
}

/*
  Function: gdImageWebpPtrEx

    See <gdImageWebpEx>.
*/
BGD_DECLARE(void *) gdImageWebpPtrEx (gdImagePtr im, int *size, int quality)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) {
		return NULL;
	}
	if (_gdImageWebpCtx(im, out, quality)) {
        rv = NULL;
    } else {
        rv = gdDPExtractData(out, size);
    }
	out->gd_free(out);
	return rv;
}

#else /* !HAVE_LIBWEBP */

static void _noWebpError(void)
{
	gd_error("WEBP image support has been disabled\n");
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromWebp (FILE * inFile)
{
	_noWebpError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpPtr (int size, void *data)
{
	_noWebpError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpCtx (gdIOCtx * infile)
{
	_noWebpError();
	return NULL;
}

BGD_DECLARE(void) gdImageWebpCtx (gdImagePtr im, gdIOCtx * outfile, int quality)
{
	_noWebpError();
}

BGD_DECLARE(void) gdImageWebpEx (gdImagePtr im, FILE * outFile, int quality)
{
	_noWebpError();
}

BGD_DECLARE(void) gdImageWebp (gdImagePtr im, FILE * outFile)
{
	_noWebpError();
}

BGD_DECLARE(void *) gdImageWebpPtr (gdImagePtr im, int *size)
{
	_noWebpError();
	return NULL;
}

BGD_DECLARE(void *) gdImageWebpPtrEx (gdImagePtr im, int *size, int quality)
{
	_noWebpError();
	return NULL;
}

#endif /* HAVE_LIBWEBP */
