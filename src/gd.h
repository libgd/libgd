#ifndef GD_H
#define GD_H 1

#ifdef __cplusplus
extern "C" {
#endif

/* gd.h: declarations file for the graphic-draw module.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  This software is provided "AS IS." Thomas Boutell and
 * Boutell.Com, Inc. disclaim all warranties, either express or implied, 
 * including but not limited to implied warranties of merchantability and 
 * fitness for a particular purpose, with respect to this code and accompanying
 * documentation. */

/* stdio is needed for file I/O. */
#include <stdio.h>
#include "gd_io.h"

/* This can't be changed in the current palette-only version of gd. */

#define gdMaxColors 256

/* Image type. See functions below; you will not need to change
	the elements directly. Use the provided macros to
	access sx, sy, the color table, and colorsTotal for 
	read-only purposes. */

typedef struct gdImageStruct {
	unsigned char ** pixels;
	int sx;
	int sy;
	int colorsTotal;
	int red[gdMaxColors];
	int green[gdMaxColors];
	int blue[gdMaxColors]; 
	int open[gdMaxColors];
	int transparent;
	int *polyInts;
	int polyAllocated;
	struct gdImageStruct *brush;
	struct gdImageStruct *tile;	
	int brushColorMap[gdMaxColors];
	int tileColorMap[gdMaxColors];
	int styleLength;
	int stylePos;
	int *style;
	int interlace;
} gdImage;

typedef gdImage * gdImagePtr;

typedef struct {
	/* # of characters in font */
	int nchars;
	/* First character is numbered... (usually 32 = space) */
	int offset;
	/* Character width and height */
	int w;
	int h;
	/* Font data; array of characters, one row after another.
		Easily included in code, also easily loaded from
		data files. */
	char *data;
} gdFont;

/* Text functions take these. */
typedef gdFont *gdFontPtr;

/* For backwards compatibility only. Use gdImageSetStyle()
	for MUCH more flexible line drawing. Also see
	gdImageSetBrush(). */
#define gdDashSize 4

/* Special colors. */

#define gdStyled (-2)
#define gdBrushed (-3)
#define gdStyledBrushed (-4)
#define gdTiled (-5)

/* NOT the same as the transparent color index.
	This is used in line styles only. */
#define gdTransparent (-6)

/* Functions to manipulate images. */

gdImagePtr gdImageCreate(int sx, int sy);
gdImagePtr gdImageCreateFromPng(FILE *fd);
gdImagePtr gdImageCreateFromPngCtx(gdIOCtxPtr in);

/* A custom data source. */
/* The source function must return -1 on error, otherwise the number
        of bytes fetched. 0 is EOF, not an error! */
/* context will be passed to your source function. */

typedef struct {
        int (*source) (void *context, char *buffer, int len);
        void *context;
} gdSource, *gdSourcePtr;

gdImagePtr gdImageCreateFromPngSource(gdSourcePtr in);

gdImagePtr gdImageCreateFromGd(FILE *in);
gdImagePtr gdImageCreateFromGdCtx(gdIOCtxPtr in);

gdImagePtr gdImageCreateFromGd2(FILE *in);
gdImagePtr gdImageCreateFromGd2Ctx(gdIOCtxPtr in);

gdImagePtr gdImageCreateFromGd2Part(FILE *in, int srcx, int srcy, int w, int h);
gdImagePtr gdImageCreateFromGd2PartCtx(gdIOCtxPtr in, int srcx, int srcy, int w, int h);

gdImagePtr gdImageCreateFromXbm(FILE *fd);

void gdImageDestroy(gdImagePtr im);
void gdImageSetPixel(gdImagePtr im, int x, int y, int color);
int gdImageGetPixel(gdImagePtr im, int x, int y);
void gdImageLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
/* For backwards compatibility only. Use gdImageSetStyle()
	for much more flexible line drawing. */
void gdImageDashedLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
/* Corners specified (not width and height). Upper left first, lower right
 	second. */
void gdImageRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
/* Solid bar. Upper left corner first, lower right corner second. */
void gdImageFilledRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
int gdImageBoundsSafe(gdImagePtr im, int x, int y);
void gdImageChar(gdImagePtr im, gdFontPtr f, int x, int y, int c, int color);
void gdImageCharUp(gdImagePtr im, gdFontPtr f, int x, int y, int c, int color);
void gdImageString(gdImagePtr im, gdFontPtr f, int x, int y, unsigned char *s, int color);
void gdImageStringUp(gdImagePtr im, gdFontPtr f, int x, int y, unsigned char *s, int color);
void gdImageString16(gdImagePtr im, gdFontPtr f, int x, int y, unsigned short *s, int color);
void gdImageStringUp16(gdImagePtr im, gdFontPtr f, int x, int y, unsigned short *s, int color);

char *gdImageStringTTF(gdImage *im, int *brect, int fg, char *fontname,
                double ptsize, double angle, int x, int y, char *string);

/* Point type for use in polygon drawing. */

typedef struct {
	int x, y;
} gdPoint, *gdPointPtr;

void gdImagePolygon(gdImagePtr im, gdPointPtr p, int n, int c);
void gdImageFilledPolygon(gdImagePtr im, gdPointPtr p, int n, int c);

int gdImageColorAllocate(gdImagePtr im, int r, int g, int b);
int gdImageColorClosest(gdImagePtr im, int r, int g, int b);
int gdImageColorExact(gdImagePtr im, int r, int g, int b);
int gdImageColorResolve(gdImagePtr im, int r, int g, int b);
void gdImageColorDeallocate(gdImagePtr im, int color);
void gdImageColorTransparent(gdImagePtr im, int color);
void gdImagePaletteCopy(gdImagePtr dst, gdImagePtr src);
void gdImagePng(gdImagePtr im, FILE *out);
void gdImagePngCtx(gdImagePtr im, gdIOCtx *out);

/* A custom data sink. */
/* The sink function must return -1 on error, otherwise the number
        of bytes written, which must be equal to len. */
/* context will be passed to your sink function. */
typedef struct {
        int (*sink) (void *context, const char *buffer, int len);
        void *context;
} gdSink, *gdSinkPtr;

void gdImagePngToSink(gdImagePtr im, gdSinkPtr out);

void gdImageGd(gdImagePtr im, FILE *out);
void gdImageGd2(gdImagePtr im, FILE *out, int cs, int fmt);
void* gdImagePngPtr(gdImagePtr im, int *size);
void* gdImageGdPtr(gdImagePtr im, int *size);
void* gdImageGd2Ptr(gdImagePtr im, int cs, int fmt, int *size);
void gdImageArc(gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color);
void gdImageFillToBorder(gdImagePtr im, int x, int y, int border, int color);
void gdImageFill(gdImagePtr im, int x, int y, int color);
void gdImageCopy(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h);
void gdImageCopyMerge(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, 
			int srcX, int srcY, int w, int h, int pct);
void gdImageCopyMergeGray(gdImagePtr dst, gdImagePtr src, int dstX, int dstY,
                        int srcX, int srcY, int w, int h, int pct);

/* Stretches or shrinks to fit, as needed */
void gdImageCopyResized(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH);
void gdImageSetBrush(gdImagePtr im, gdImagePtr brush);
void gdImageSetTile(gdImagePtr im, gdImagePtr tile);
void gdImageSetStyle(gdImagePtr im, int *style, int noOfPixels);
/* On or off (1 or 0) */
void gdImageInterlace(gdImagePtr im, int interlaceArg);

/* Macros to access information about images. READ ONLY. Changing
	these values will NOT have the desired result. */
#define gdImageSX(im) ((im)->sx)
#define gdImageSY(im) ((im)->sy)
#define gdImageColorsTotal(im) ((im)->colorsTotal)
#define gdImageRed(im, c) ((im)->red[(c)])
#define gdImageGreen(im, c) ((im)->green[(c)])
#define gdImageBlue(im, c) ((im)->blue[(c)])
#define gdImageGetTransparent(im) ((im)->transparent)
#define gdImageGetInterlaced(im) ((im)->interlace)

/* I/O Support routines. */

gdIOCtx* gdNewFileCtx(FILE*);
gdIOCtx* gdNewDynamicCtx(int, void*);
gdIOCtx* gdNewSSCtx(gdSourcePtr in, gdSinkPtr out);
void* gdDPExtractData(struct gdIOCtx* ctx, int *size);


#define GD2_CHUNKSIZE           128 
#define GD2_CHUNKSIZE_MIN	64
#define GD2_CHUNKSIZE_MAX       4096

#define GD2_VERS                1
#define GD2_ID                  "gd2"
#define GD2_FMT_RAW             1
#define GD2_FMT_COMPRESSED      2

/* Image comparison definitions */
int gdImageCompare(gdImagePtr im1, gdImagePtr im2);

#define GD_CMP_IMAGE		1	/* Actual image IS different */
#define GD_CMP_NUM_COLORS	2	/* Number of Colours in pallette differ */
#define GD_CMP_COLOR		4	/* Image colours differ */
#define GD_CMP_SIZE_X		8	/* Image width differs */
#define GD_CMP_SIZE_Y		16	/* Image heights differ */
#define GD_CMP_TRANSPARENT	32	/* Transparent colour */
#define GD_CMP_BACKGROUND	64	/* Background colour */
#define GD_CMP_INTERLACE	128	/* Interlaced setting */

#ifdef __cplusplus
}
#endif

#endif /* GD_H */
