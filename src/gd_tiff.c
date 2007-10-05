/*
   TIFF - Tagged Image File Format Encapsulation for GD Library

   gd_tiff.c

   Copyright (C) M. Retallack

   ---------------------------------------------------------------------------

   **
   ** Permission to use, copy, modify, and distribute this software and its
   ** documentation for any purpose and without fee is hereby granted, provided
   ** that the above copyright notice appear in all copies and that both that
   ** copyright notice and this permission notice appear in supporting
   ** documentation.  This software is provided "as is" without express or
   ** implied warranty.
   **
   ---------------------------------------------------------------------------

   Todo:

   If we fail - cleanup
   Writer: Use gd error function, overflow check may not be necessary as
	 we write our own data (check already done)

   (suggestions only)
   Implement 2 color black/white saving using group4 fax compression
   Implement function to specify encoding to use when writing tiff data

   ----------------------------------------------------------------------------
 */
/* $Id$ */

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <gd.h>
#include <gdfonts.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#ifdef HAVE_LIBTIFF

#include "tiff.h"
#include "tiffio.h"

typedef struct tiff_handle
{
	int size;
	int pos;
	gdIOCtx *ctx;
	int written;
}
tiff_handle;

/*
   Functions for reading, writing and seeking in gdIOCtx
   This allows for non-file i/o operations with no
   explicit use of libtiff fileio wrapper functions

   Note: because libtiff requires random access, but gdIOCtx
         only supports streams, all writes are buffered
         into memory and written out on close, also all
         reads are done from a memory mapped version of the
         tiff (assuming one already exists)
*/

tiff_handle *new_tiff_handle(tiff_handle *t, gdIOCtx *g)
{
	t = (tiff_handle *) gdMalloc(sizeof(tiff_handle));
	if(!t || !g) {
		return 0;
	}

	t->size = 0;
	t->pos = 0;
	t->ctx = g;
	t->written = 0;

	return t;
}

/* TIFFReadWriteProc tiff_readproc - Will use gdIOCtx procs to read required 
   (previously written) TIFF file content */
tsize_t tiff_readproc(thandle_t clientdata, tdata_t data, tsize_t size)
{
	tiff_handle *th = (tiff_handle *)clientdata;
	gdIOCtx *ctx = th->ctx;

	size = (ctx->getBuf)(ctx, data, size);

	return size;
}

/* TIFFReadWriteProc tiff_writeproc - Will use gdIOCtx procs to write out 
   TIFF data */
tsize_t tiff_writeproc(thandle_t clientdata, tdata_t data, tsize_t size)
{
	tiff_handle *th = (tiff_handle *)clientdata;
	gdIOCtx *ctx = th->ctx;

	size = (ctx->putBuf)(ctx, data, size);
	if(size + th->pos>th->size) {
		th->size = size + th->pos;
		th->pos += size;
	}

	return size;
}

/* TIFFSeekProc tiff_seekproc
 * used to move around the partially written TIFF */
toff_t tiff_seekproc(thandle_t clientdata, toff_t offset, int from)
{
	tiff_handle *th = (tiff_handle *)clientdata;
	gdIOCtx *ctx = th->ctx;
	int result;

	switch(from) {
		default:
		case SEEK_SET:
			/* just use offset */
		break;

		case SEEK_END:
			/* invert offset, so that it is from start, not end as supplied */
			offset = th->size + offset;
		break;

		case SEEK_CUR:
			/* add current position to translate it to 'from start', 
			 * not from durrent as supplied
			 */
			offset += th->pos;
		break;
	}

	/* now, move pos in both io context and buf */
	if((result = (ctx->seek)(ctx, offset))) {
		th->pos = offset;
	}

	return result ? offset : -1;
}

/* TIFFCloseProc tiff_closeproc - used to finally close the TIFF file */
int tiff_closeproc(thandle_t clientdata)
{
	tiff_handle *th = (tiff_handle *)clientdata;
	gdIOCtx *ctx = th->ctx;

	/*(ctx->gd_free)(ctx);*/

	return 0;
}

/* TIFFSizeProc tiff_sizeproc */
toff_t tiff_sizeproc(thandle_t clientdata)
{
	tiff_handle *th = (tiff_handle *)clientdata;
	return th->size;
}

/* TIFFMapFileProc tiff_mapproc() */
int tiff_mapproc(thandle_t h, tdata_t *d, toff_t *o)
{
	return 0;
}

/* TIFFUnmapFileProc tiff_unmapproc */
void tiff_unmapproc(thandle_t h, tdata_t d, toff_t o)
{

}


/*  tiffWriter 
**  ----------
**  Write the gd image as a tiff file (called by gdImageTiffCtx)
**  Parameters are:
**  image:    gd image structure;
**  out:      the stream where to write
**  bitDepth: depth in bits of each pixel
 */
BGD_DECLARE(void) tiffWriter(gdImagePtr image, gdIOCtx *out, int bitDepth)
{
	int x, y;
	int i;
	int r, g, b, a;
	TIFF *tiff;
	int width;
	int height;
	int colour;
	char *scan;
	int rowsperstrip;
	int samplesPerPixel = 3;
	int bitsPerSample;
	int transparentColourR = -1;
	int transparentColourG = -1;
	int transparentColourB = -1;
	uint16 extraSamples[1];
	uint16 *colorMapRed;
	uint16 *colorMapGreen;
	uint16 *colorMapBlue;

	tiff_handle *th;
	th = new_tiff_handle(th, out);
	extraSamples[0] = EXTRASAMPLE_ASSOCALPHA;

	/* read in the width/height of gd image */
	width = gdImageSX(image);
	height = gdImageSY(image);

	/* reset clip region to whole image */
	gdImageSetClip(image, 0, 0, width, height);

	/* handle old-style single-colour mapping to 100% transparency */
	if(image->transparent != 0xffffffff) {
		/* set our 100% transparent colour value */
		transparentColourR = gdImageRed(image, image->transparent);
		transparentColourG = gdImageGreen(image, image->transparent);
		transparentColourB = gdImageBlue(image, image->transparent);
	}

	/* Open tiff file writing routines, but use special read/write/seek
	 * functions so that tiff lib writes correct bits of tiff content to
	 * correct areas of file opened and modifieable by the gdIOCtx functions
	 */
	tiff = TIFFClientOpen("", "w", th,	tiff_readproc,
										tiff_writeproc,
										tiff_seekproc,
										tiff_closeproc,
										tiff_sizeproc,
										tiff_mapproc,
										tiff_unmapproc);

	TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);
	TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
	TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC,
					(bitDepth == 24) ? PHOTOMETRIC_RGB : PHOTOMETRIC_PALETTE);

	bitsPerSample = (bitDepth ==24) ? 8 : ((bitDepth == 8) ? 8 : 1);
	TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, bitsPerSample);

	/* build the color map for 8 bit images */
	if(bitDepth != 24) {
		/*TODO: Add checking */
		colorMapRed = (uint16 *) gdMalloc(3 * pow(2, bitsPerSample));
		colorMapGreen = (uint16 *) gdMalloc(3 * pow(2, bitsPerSample));
		colorMapBlue = (uint16 *) gdMalloc(3 * pow(2, bitsPerSample));

		for(i = 0; i < image->colorsTotal; i++) {
			colorMapRed[i] = gdImageRed(image,i) + (gdImageRed(image,i) * 256);
			colorMapGreen[i] =gdImageGreen(image,i)+(gdImageGreen(image,i)*256);
			colorMapBlue[i] = gdImageBlue(image,i) + (gdImageBlue(image,i)*256);
		}

		TIFFSetField(tiff, TIFFTAG_COLORMAP, colorMapRed, colorMapGreen,
															colorMapBlue);
		samplesPerPixel = 1;
	}

	/* here, we check if the 'save alpha' flag is set on the source gd image */
	if(	(bitDepth == 24) &&
		(image->saveAlphaFlag || image->transparent != 0xffffffff)) {
		/* so, we need to store the alpha values too! 
		 * Also, tell TIFF what the extra sample means (associated alpha) */
		samplesPerPixel = 4;
		TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
		TIFFSetField(tiff, TIFFTAG_EXTRASAMPLES, 1, extraSamples);
	} else {
		TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
	}

	TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, 1);

	if(overflow2(width, samplesPerPixel)) {
		return;
	}

	if(!(scan = (char *)gdMalloc(width * samplesPerPixel))) {
		return;
	}

	/* loop through y-coords, and x-coords */
	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			/* generate scan line for writing to tiff */
			colour = gdImageGetPixel(image, x, y);

			a = (127 - gdImageAlpha(image, colour)) * 2;
			a = (a == 0xfe) ? 0xff : a & 0xff;
			b = gdImageBlue(image, colour);
			g = gdImageGreen(image, colour);
			r = gdImageRed(image, colour);

			/* if this pixel has the same RGB as the transparent colour,
			 * then set alpha fully transparent */
			if(	transparentColourR == r &&
				transparentColourG == g &&
				transparentColourB == b
			) {
				a = 0x00;
			}

			if(bitDepth != 24) {
				/* write out 1 or 8 bit value in 1 byte
				 * (currently treats 1bit as 8bit) */
				scan[(x * samplesPerPixel) + 0] = colour;
			} else {
				/* write out 24 bit value in 3 (or 4 if transparent) bytes */
				if(image->saveAlphaFlag || image->transparent != 0xffffffff) {
					scan[(x * samplesPerPixel) + 3] = a;
				}

				scan[(x * samplesPerPixel) + 2] = b;
				scan[(x * samplesPerPixel) + 1] = g;
				scan[(x * samplesPerPixel) + 0] = r;
			}
		}

		/* Write the scan line to the tiff */
		if(TIFFWriteEncodedStrip(tiff, y, scan, width * samplesPerPixel) == -1){
			/* error handler here */
			fprintf(stderr, "Could not create TIFF\n");
			return;
		}
	}

	/* now cloase and free up resources */
	TIFFClose(tiff);
	gdFree(scan);
	gdFree(th);

	if(bitDepth != 24) {
		gdFree(colorMapRed);
		gdFree(colorMapGreen);
		gdFree(colorMapBlue);
	}
}

/*  gdImageTiffCtx
**  --------------
**  Write the gd image as a tiff file
**  Parameters are:
**  image:  gd image structure;
**  out:    the stream where to write
 */
BGD_DECLARE(void) gdImageTiffCtx(gdImagePtr image, gdIOCtx *out)
{
	int clipx1P, clipy1P, clipx2P, clipy2P;
	int bitDepth = 24;

	/* First, switch off clipping, or we'll not get all the image! */
	gdImageGetClip(image, &clipx1P, &clipy1P, &clipx2P, &clipy2P);

	/* use the appropriate routine depending on the bit depth of the image */
	if(image->trueColor) {
		bitDepth = 24;
	} else if(image->colorsTotal == 2) {
		bitDepth = 1;
	} else {
		bitDepth = 8;
	}

	tiffWriter(image, out, bitDepth);

	/* reset clipping area to the gd image's original values */
	gdImageSetClip(image, clipx1P, clipy1P, clipx2P, clipy2P);
}


/* readColorMap
** ------------
** Used by tiff reading routines to generate a map between color indices
** and their actual RGB color values.
** return value is a chunk of memory containing 
 */

typedef struct
{
	uint16 *red;
	uint16 *green;
	uint16 *blue;
}
RgbContext;

void readColorMap(TIFF *tiff, RgbContext ctx)
{
	uint32 dataSize;

	TIFFGetField(tiff, TIFFTAG_COLORMAP, &ctx.red, &ctx.green, &ctx.blue);
}

/* getRed, getGreen, getBlue
** -------------------------
** gets the red value for specified index from specified color map data 
** from source tiff file 
 */
BGD_DECLARE(uint16) getColor(TIFF *tiff, RgbContext ctx, int index, char color)
{
	uint16 bitsPerSample;
	uint16 val;
	uint32 offset;

	TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
	offset = pow(2, bitsPerSample);

	if(color == 0) {
		val = ctx.red[index];
	}
	if(color == 1) {
		val = ctx.green[index];
	}
	if(color == 2) {
		val = ctx.blue[index];
	}

	val = (val >> 8);

	return val;
}

#define getRed(t,c,i) getColor(t,c,i,0)
#define getGreen(t,c,i) getColor(t,c,i,1)
#define getBlue(t,c,i) getColor(t,c,i,2)

/* createFromTiffCtx1bit
** ------------------------
** Create a gdImage from a TIFF file input from an gdIOCtx
** called by gdImageCreateFromTiffCtx() when bitsPerPixel==1
 */
BGD_DECLARE(gdImagePtr) createFromTiffCtx1bit(TIFF *tiff, int width, int height)
{
	gdImagePtr im = NULL;
	int x, y;
	int tileX, tileY, tileMaxX, tileMaxY;
	int i, j, k;
	int colour;
	char *scan;
	int compression;
	ttile_t numberOfTiles = 0;
	int tileNum;
	uint32 black, white;
	unsigned long long *bitmapAsVal;
	unsigned long long bitToTest;
	unsigned long long bitTestResult;
	unsigned char byteVal;
	unsigned char byteBitToTest;
	unsigned char byteBitTestResult;
	uint32 tileSize;
	RgbContext ctx;

	TIFFGetField(tiff, TIFFTAG_COMPRESSION, &compression);
 
	if(!(im = gdImageCreate(width, height))) {
		TIFFClose(tiff);
		return NULL;
	}

	/* allocate both black and white for this image */
	white = gdImageColorAllocate(im, 0xff, 0xff, 0xff); 
	black = gdImageColorAllocate(im, 0x00, 0x00, 0x00);

	numberOfTiles = TIFFNumberOfTiles(tiff);

	if(TIFFIsTiled(tiff)) {
		TIFFGetField(tiff, TIFFTAG_TILEWIDTH, &tileMaxX);
		TIFFGetField(tiff, TIFFTAG_TILELENGTH, &tileMaxY);

		tileSize = TIFFTileSize(tiff);
		if(!(scan = (char *) gdMalloc(tileSize))) {
			return 0;
		}

		tileX = tileY = 0;

		for(tileNum = 0; tileNum < numberOfTiles; tileNum++) {
			TIFFReadEncodedTile(tiff, tileNum, scan, tileSize); 

			/* setup coords for this pixel */
			x = tileX;
			y = tileY;

			for(i=0; i < tileSize; i += sizeof(unsigned long long)) {
				/* check if the next sizeof(unsigned long long) bytes are zero,
				 * if they are, move along */
				bitmapAsVal = (unsigned long long *)&scan[i];
				if(bitmapAsVal[0] != 0ull) {
					/* if this chunk has some black bits, write them out,
					 * one bit at a time */
					bitToTest = 1ull;

					for(j = 0; j < sizeof(unsigned long long) * 8; j++) {
						bitTestResult = bitmapAsVal[0] & bitToTest;

						/* if the bit at offset j is white, skip it */
						if(bitTestResult != 0ull) {
							gdImageSetPixel(im, x, y, black);
						}

						/* move onto next pixel in tile's gd target area */
						x++;
						if(x >= (tileX + tileMaxX)) {
							x = tileX;
							y++;
						}

						bitToTest *= 2;
					}
				} else {
					/* entire section was blank - move x on by section length */
					x += sizeof(unsigned long long) * 8;
					while(x >= (tileX + tileMaxX)) {
						x -= tileMaxX;
						y++;
					}
				}
			}
      
			/* move to the next tile offset */
			tileX += tileMaxX;
			if(tileX > width) {
				tileX = 0;
				tileY += tileMaxY;
			}
		}

		gdFree(scan);

	} else {
		/* image is not tiled - assume stripped (scanline) format */
		printf("is a 1bit scanline tiff!\n");

		if(!(scan = (char *) gdMalloc(TIFFScanlineSize(tiff)))) {
			return 0;
		}

		y = x = 0;

		for(i = 0; i < height; i++) {
			/* looping on number of scanlines - read source scanline */
			TIFFReadScanline(tiff, scan, i, 0);

			for(j = 0; j < TIFFScanlineSize(tiff); j++) {
				/* now, loop on content of scanline */
				byteVal = scan[j];
				byteBitToTest = 0x01;

				if(byteVal == 0) {
					x += 8;
					continue;
				}

				/* now, loop through this byte, setting all bits that are not
				 * the background color */
				for(k = 0; k < 8; k++) {
					byteBitTestResult = (byteVal & byteBitToTest);

					if(byteBitTestResult) {
						gdImageSetPixel(im, x, y, black);
					}

					x++;
					byteBitToTest *= 2;
				}
			}

			y++;
			x = 0;
		}

		gdFree(scan);
	}

	return im;
}

/* createFromTiffCtx8bit
** ------------------------
** Create a gdImage from a TIFF file input from an gdIOCtx
** called by gdImageCreateFromTiffCtx() when bitsPerPixel==8
 */
BGD_DECLARE(gdImagePtr) createFromTiffCtx8bit(TIFF *tiff, int width, int height)
{
	gdImagePtr im = NULL;
	int x, y;
	int i, j;
	int colorIndex;
	int colour;
	char *scan;
	uint32 numberOfTiles = 0;
	uint32 tileSize;
	int tileX, tileY;
	int tileMaxX, tileMaxY;
	int tileNum;
	uint32 color;
	char *colorMap;
	RgbContext ctx;

	if(!(im = gdImageCreate(width, height))) {
		TIFFClose(tiff);
		return NULL;
	}

	readColorMap(tiff, ctx);

	/* set the background of the image to the first color in the color map */
	gdImageColorAllocate(im, getRed(tiff, ctx, 0),
							getGreen(tiff, ctx, 0),
							getBlue(tiff, ctx, 0));

	numberOfTiles = TIFFNumberOfTiles(tiff);

	if(TIFFIsTiled(tiff)) {
		printf("is a 8bit tiled tiff! (%d tiles)\n",numberOfTiles);
		TIFFGetField(tiff, TIFFTAG_TILEWIDTH, &tileMaxX);
		TIFFGetField(tiff, TIFFTAG_TILELENGTH, &tileMaxY);

		tileSize = TIFFTileSize(tiff);
		if(!(scan = (char*) gdMalloc(tileSize))) {
			return 0;
		}

		tileX = tileY = 0;

		for(tileNum = 0; tileNum < numberOfTiles; tileNum++) {
			TIFFReadEncodedTile(tiff, tileNum, scan, tileSize);

			/* setup coords for this pixel */
			x = tileX;
			y = tileY;

			for(i = 0; i < tileSize; i++) {
				/* check if the next byte is zero */
				/* if it is - background already set so move along */
				colorIndex = scan[i];
				if(colorIndex != 0x0) {
					color = gdImageColorResolve(im,
											getRed(tiff, ctx, colorIndex),
											getGreen(tiff, ctx, colorIndex),
											getBlue(tiff, ctx, colorIndex));

					gdImageSetPixel(im, x, y, color);

					/* move onto next pixel in tile's gd target area */
					x++;
					if(x >= (tileX + tileMaxX)) {
						x = tileX;
						y++;
					}
				} else {
					/* move x on to next pixel offset */
					x++;
					if(x >= (tileX + tileMaxX)) {
						x = tileMaxX;
						y++;
					}
				}
			}

			/* move to the next tile offset */
			tileX += tileMaxX;
			if(tileX > width) {
				tileX = 0;
				tileY += tileMaxY;
			}
		}

		gdFree(scan);

	} else {
		if(!(scan = (char *)gdMalloc(TIFFScanlineSize(tiff)))) {
			return 0;
		}

		y = x = 0;

		/* image is not tiled - assume scanline based images */
		for(i = 0; i < height; i++) {
			/* looping on number of scanlines - read source scanline */
			TIFFReadScanline(tiff, scan, i, 0);

			for(j = 0; j < TIFFScanlineSize(tiff); j++) {
				colorIndex = scan[j];
				color = gdImageColorResolve(im, getRed(tiff, ctx, colorIndex),
												getGreen(tiff, ctx, colorIndex),
												getBlue(tiff, ctx, colorIndex));

				/*printf("%2x  %2x %2x %2x\n",colorIndex,getRed(tiff,colorIndex),getGreen(tiff,colorIndex),getBlue(tiff,colorIndex));*/

				gdImageSetPixel(im,x,y,color);
				x++;
			}
			
			y++;
			x = 0;
		}

		gdFree(scan);
	}

	return im;
}

/* createFromTiffCtx32bit
** ------------------------
** Create a 32 bit gdImage from a TIFF file input from an gdIOCtx
** - called by gdImageCreateFromTiffCtx() when bitsPerPixel>8bit
** This is essentially a fall-back routine; it's very slow but will 
** handle any format of tiff that libtiff supports.
 */
BGD_DECLARE(gdImagePtr) createFromTiffCtx32bit(TIFF *tiff, int width,int height)
{
	gdImagePtr im = NULL;
	int r, g, b, a;
	int x, y;
	int alphaBlendingFlag = 0;
	int colour;
	uint32 *scan;
	uint32 rgba;

	printf("32 bits\n");

	if(!(im = gdImageCreateTrueColor(width, height))) {
		TIFFClose(tiff);
		return NULL;
	}

	/* switch off colour merging on target gd image just while we write out
	 * content - we want to preserve the alpha data until the user chooses
	 * what to do with the image */
	alphaBlendingFlag = im->alphaBlendingFlag;
	gdImageAlphaBlending(im, 0);

	/* loop through y-coords, and x-coords */
	scan = calloc(sizeof(uint32), width * height);
	TIFFReadRGBAImage(tiff, width, height, scan, 0);

	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			/* if it doesn't already exist, allocate a new colour,
			 * else use existing one */
			rgba = scan[((y * width) + x)];
			a = (0xff - TIFFGetA(rgba)) / 2;

			b = TIFFGetB(rgba);
			g = TIFFGetG(rgba);
			r = TIFFGetR(rgba);

			colour = gdTrueColorAlpha(r, g, b, a);

			/* set pixel colour to this colour */
			gdImageSetPixel(im, x, height - y - 1, colour);
		}
	}

	gdFree(scan);

	/* now reset colour merge for alpha blending routines */
	gdImageAlphaBlending(im, alphaBlendingFlag);

	return im;
}

/* gdImageCreateFromTiffCtx
** ------------------------
** Create a gdImage from a TIFF file input from an gdIOCtx
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromTiffCtx(gdIOCtx *infile)
{
	int width;
	int height;
	int compression;
	uint16 samplesPerPixel;
	uint16 bitsPerSample[4];
	int bitsPerPixel;
	TIFF *tiff;
	gdImagePtr im;

	tiff_handle *th;
	th = new_tiff_handle(th, infile);

	tiff = TIFFClientOpen("", "rb", th, tiff_readproc,
										tiff_writeproc,
										tiff_seekproc,
										tiff_closeproc,
										tiff_sizeproc,
										tiff_mapproc,
										tiff_unmapproc);


	TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, bitsPerSample);
	TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
	bitsPerPixel = bitsPerSample[0] * samplesPerPixel;

	if(bitsPerPixel == 1) {
		im = createFromTiffCtx1bit(tiff, width, height);
	} else if(bitsPerPixel == 8) {
		im = createFromTiffCtx8bit(tiff, width, height);
	} else {
		im = createFromTiffCtx32bit(tiff, width, height);
	}

	TIFFClose(tiff);
	gdFree(th);

	return im;
}

/* gdImageCreateFromTIFF
** ---------------------
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromTiff(FILE *inFile)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx(inFile);
	im = gdImageCreateFromTiffCtx(in);
	in->gd_free(in);
	return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromTiffPtr(int size, void *data)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx (size, data, 0);
	im = gdImageCreateFromTiffCtx(in);
	in->gd_free(in);
	return im;
}

/* gdImageTIFF
** -----------
 */
BGD_DECLARE(void) gdImageTiff(gdImagePtr im, FILE *outFile)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	gdImageTiffCtx(im, out); /* what's an fg again? */
	out->gd_free(out);
}

/* gdImageTIFFPtr
** --------------
 */
BGD_DECLARE(void *) gdImageTiffPtr(gdImagePtr im, int *size)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx (2048, NULL);
	gdImageTiffCtx(im, out); /* what's an fg again? */
	rv = gdDPExtractData(out, size);
	out->gd_free(out);
	return rv;
}

#endif
