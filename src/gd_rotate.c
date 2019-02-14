#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "gd.h"
#include "gd_intern.h"
#include <math.h>

/*
 * Rotate function Added on 2003/12
 * by Pierre-Alain Joye (pierre@php.net)
 **/
/* Begin rotate function */
#ifdef ROTATE_PI
#undef ROTATE_PI
#endif /* ROTATE_PI */

typedef int (BGD_STDCALL *FuncPtr)(gdImagePtr, int, int);

static int rot90x(gdImagePtr dst, int x, int y);
static int rot90y(gdImagePtr dst, int x, int y);

static int rot180x(gdImagePtr dst, int x, int y);
static int rot180y(gdImagePtr dst, int x, int y);

static int rot270x(gdImagePtr dst, int x, int y);
static int rot270y(gdImagePtr dst, int x, int y);

static int flipHrot90x(gdImagePtr dst, int x, int y);
static int flipHrot90y(gdImagePtr dst, int x, int y);

static int flipHrot180x(gdImagePtr dst, int x, int y);
static int flipHrot180y(gdImagePtr dst, int x, int y);

static int flipHrot270x(gdImagePtr dst, int x, int y);
static int flipHrot270y(gdImagePtr dst, int x, int y);

static int flipVrot90x(gdImagePtr dst, int x, int y);
static int flipVrot90y(gdImagePtr dst, int x, int y);

static int flipVrot180x(gdImagePtr dst, int x, int y);
static int flipVrot180y(gdImagePtr dst, int x, int y);

static int flipVrot270x(gdImagePtr dst, int x, int y);
static int flipVrot270y(gdImagePtr dst, int x, int y);

static int flipHx(gdImagePtr dst, int x, int y);
static int flipHy(gdImagePtr dst, int x, int y);

static int flipVx(gdImagePtr dst, int x, int y);
static int flipVy(gdImagePtr dst, int x, int y);

static BGD_DECLARE(gdImagePtr) gdImageRotateHelper(gdImagePtr src, int ignoretransparent, int dstW, int dstH, FuncPtr transX, FuncPtr transY);

#define ROTATE_DEG2RAD  3.1415926535897932384626433832795/180
void gdImageSkewX (gdImagePtr dst, gdImagePtr src, int uRow, int iOffset, double dWeight, int clrBack, int ignoretransparent)
{
	int i, r, g, b, a, clrBackR, clrBackG, clrBackB, clrBackA;
	FuncPtr f;

	int pxlOldLeft, pxlLeft=0, pxlSrc;

	/* Keep clrBack as color index if required */
	if (src->trueColor) {
		pxlOldLeft = clrBack;
		f = gdImageGetTrueColorPixel;
	} else {
		pxlOldLeft = clrBack;
		clrBackR = gdImageRed(src, clrBack);
		clrBackG = gdImageGreen(src, clrBack);
		clrBackB = gdImageBlue(src, clrBack);
		clrBackA = gdImageAlpha(src, clrBack);
		clrBack =  gdTrueColorAlpha(clrBackR, clrBackG, clrBackB, clrBackA);
		f = gdImageGetPixel;
	}

	for (i = 0; i < iOffset; i++) {
		gdImageSetPixel (dst, i, uRow, clrBack);
	}

	if (i < dst->sx) {
		gdImageSetPixel (dst, i, uRow, clrBack);
	}

	for (i = 0; i < src->sx; i++) {
		pxlSrc = f (src,i,uRow);

		r = (int)(gdImageRed(src,pxlSrc) * dWeight);
		g = (int)(gdImageGreen(src,pxlSrc) * dWeight);
		b = (int)(gdImageBlue(src,pxlSrc) * dWeight);
		a = (int)(gdImageAlpha(src,pxlSrc) * dWeight);

		if (r>255) {
			r = 255;
		}

		if (g>255) {
			g = 255;
		}

		if (b>255) {
			b = 255;
		}

		if (a>127) {
			a = 127;
		}

		pxlLeft = gdTrueColorAlpha(r, g, b, a);

		r = gdImageRed(src,pxlSrc)   - (r - gdImageRed(src,pxlOldLeft));
		g = gdImageGreen(src,pxlSrc) - (g - gdImageGreen(src,pxlOldLeft));
		b = gdImageBlue(src,pxlSrc)  - (b - gdImageBlue(src,pxlOldLeft));
		a = gdImageAlpha(src,pxlSrc) - (a - gdImageAlpha(src,pxlOldLeft));

		if (r>255) {
			r = 255;
		}

		if (g>255) {
			g = 255;
		}

		if (b>255) {
			b = 255;
		}

		if (a>127) {
			a = 127;
		}

		if (ignoretransparent && pxlSrc == dst->transparent) {
			pxlSrc = dst->transparent;
		} else {
			pxlSrc = gdImageColorAllocateAlpha(dst, r, g, b, a);

			if (pxlSrc == -1) {
				pxlSrc = gdImageColorClosestAlpha(dst, r, g, b, a);
			}
		}

		if ((i + iOffset >= 0) && (i + iOffset < dst->sx)) {
			gdImageSetPixel (dst, i+iOffset, uRow,  pxlSrc);
		}

		pxlOldLeft = pxlLeft;
	}

	i += iOffset;

	if (i < dst->sx) {
		gdImageSetPixel (dst, i, uRow, pxlLeft);
	}

	gdImageSetPixel (dst, iOffset, uRow, clrBack);

	i--;

	while (++i < dst->sx) {
		gdImageSetPixel (dst, i, uRow, clrBack);
	}
}

void gdImageSkewY (gdImagePtr dst, gdImagePtr src, int uCol, int iOffset, double dWeight, int clrBack, int ignoretransparent)
{
	int i, iYPos=0, r, g, b, a;
	FuncPtr f;
	int pxlOldLeft, pxlLeft=0, pxlSrc;

	if (src->trueColor) {
		f = gdImageGetTrueColorPixel;
	} else {
		f = gdImageGetPixel;
	}

	for (i = 0; i<=iOffset; i++) {
		gdImageSetPixel (dst, uCol, i, clrBack);
	}
	r = (int)((double)gdImageRed(src,clrBack) * dWeight);
	g = (int)((double)gdImageGreen(src,clrBack) * dWeight);
	b = (int)((double)gdImageBlue(src,clrBack) * dWeight);
	a = (int)((double)gdImageAlpha(src,clrBack) * dWeight);

	pxlOldLeft = gdImageColorAllocateAlpha(dst, r, g, b, a);

	for (i = 0; i < src->sy; i++) {
		pxlSrc = f (src, uCol, i);
		iYPos = i + iOffset;

		r = (int)((double)gdImageRed(src,pxlSrc) * dWeight);
		g = (int)((double)gdImageGreen(src,pxlSrc) * dWeight);
		b = (int)((double)gdImageBlue(src,pxlSrc) * dWeight);
		a = (int)((double)gdImageAlpha(src,pxlSrc) * dWeight);

		if (r>255) {
        	r = 255;
		}

		if (g>255) {
			g = 255;
		}

		if (b>255) {
    			b = 255;
		}

		if (a>127) {
			a = 127;
		}

		pxlLeft = gdTrueColorAlpha(r, g, b, a);

		r = gdImageRed(src,pxlSrc)   - (r - gdImageRed(src,pxlOldLeft));
		g = gdImageGreen(src,pxlSrc) - (g - gdImageGreen(src,pxlOldLeft));
		b = gdImageBlue(src,pxlSrc)  - (b - gdImageBlue(src,pxlOldLeft));
		a = gdImageAlpha(src,pxlSrc) - (a - gdImageAlpha(src,pxlOldLeft));

		if (r>255) {
        	r = 255;
		}

		if (g>255) {
			g = 255;
		}

		if (b>255) {
    			b = 255;
		}

		if (a>127) {
			a = 127;
		}

		if (ignoretransparent && pxlSrc == dst->transparent) {
			pxlSrc = dst->transparent;
		} else {
			pxlSrc = gdImageColorAllocateAlpha(dst, r, g, b, a);

			if (pxlSrc == -1) {
				pxlSrc = gdImageColorClosestAlpha(dst, r, g, b, a);
			}
		}

		if ((iYPos >= 0) && (iYPos < dst->sy)) {
			gdImageSetPixel (dst, uCol, iYPos, pxlSrc);
		}

		pxlOldLeft = pxlLeft;
	}

	i = iYPos;
	if (i < dst->sy) {
		gdImageSetPixel (dst, uCol, i, pxlLeft);
	}

	i--;
	while (++i < dst->sy) {
		gdImageSetPixel (dst, uCol, i, clrBack);
	}
}

/* Rotates an image by 90 degrees (counter clockwise) */
BGD_DECLARE(gdImagePtr) gdImageRotate90(gdImagePtr src, int ignoretransparent)
{
	return gdImageRotateHelper(src, ignoretransparent, src->sy, src->sx, rot90x, rot90y);
}

/* Rotates an image by 180 degrees (counter clockwise) */
BGD_DECLARE(gdImagePtr) gdImageRotate180(gdImagePtr src, int ignoretransparent)
{
	return gdImageRotateHelper(src, ignoretransparent, src->sx, src->sy, rot180x, rot180y);
}

/* Rotates an image by 270 degrees (counter clockwise) */
BGD_DECLARE(gdImagePtr) gdImageRotate270(gdImagePtr src, int ignoretransparent)
{
	return gdImageRotateHelper(src, ignoretransparent, src->sy, src->sx, rot270x, rot270y);
}

/* Rotates an horizontal flipped image by 90 degrees (counter clockwise) */
BGD_DECLARE(gdImagePtr) gdImageFlipHRotate90(gdImagePtr src, int ignoretransparent)
{
	return gdImageRotateHelper(src, ignoretransparent, src->sy, src->sx, flipHrot90x, flipHrot90y);
}

/* Rotates an horizontal flipped image by 180 degrees (counter clockwise) */
BGD_DECLARE(gdImagePtr) gdImageFlipHRotate180(gdImagePtr src, int ignoretransparent)
{
	return gdImageRotateHelper(src, ignoretransparent, src->sx, src->sy, flipHrot180x, flipHrot180y);
}

/* Rotates an horizontal flipped image by 270 degrees (counter clockwise) */
BGD_DECLARE(gdImagePtr) gdImageFlipHRotate270(gdImagePtr src, int ignoretransparent)
{
	return gdImageRotateHelper(src, ignoretransparent, src->sy, src->sx, flipHrot270x, flipHrot270y);
}

/* Rotates an vertical flipped image by 90 degrees (counter clockwise) */
BGD_DECLARE(gdImagePtr) gdImageFlipVRotate90(gdImagePtr src, int ignoretransparent)
{
	return gdImageRotateHelper(src, ignoretransparent, src->sy, src->sx, flipVrot90x, flipVrot90y);
}

/* Rotates an vertical flipped image by 180 degrees (counter clockwise) */
BGD_DECLARE(gdImagePtr) gdImageFlipVRotate180(gdImagePtr src, int ignoretransparent)
{
	return gdImageRotateHelper(src, ignoretransparent, src->sx, src->sy, flipVrot180x, flipVrot180y);
}

/* Rotates an vertical flipped image by 270 degrees (counter clockwise) */
BGD_DECLARE(gdImagePtr) gdImageFlipVRotate270(gdImagePtr src, int ignoretransparent)
{
	return gdImageRotateHelper(src, ignoretransparent, src->sy, src->sx, flipVrot270x, flipVrot270y);
}

/* Flip an image horizontally */
BGD_DECLARE(gdImagePtr) gdImageFlipH(gdImagePtr src, int ignoretransparent)
{
	return gdImageRotateHelper(src, ignoretransparent, src->sx, src->sy, flipHx, flipHy);
}

/* Flip an image vertically */
BGD_DECLARE(gdImagePtr) gdImageFlipV(gdImagePtr src, int ignoretransparent)
{
	return gdImageRotateHelper(src, ignoretransparent, src->sx, src->sy, flipVx, flipVy);
}

/* Rotates an image */
static BGD_DECLARE(gdImagePtr) gdImageRotateHelper(gdImagePtr src, int ignoretransparent, int dstW, int dstH, FuncPtr transX, FuncPtr transY)
{
	int uY, uX;
	int dstX, dstY;
	int c,r,g,b,a;
	gdImagePtr dst;
	FuncPtr f;

	if (src->trueColor) {
		f = gdImageGetTrueColorPixel;
	} else {
		f = gdImageGetPixel;
	}
	dst = gdImageCreateTrueColor(dstW, dstH);
	if (dst != NULL) {
		int old_blendmode = dst->alphaBlendingFlag;
		dst->alphaBlendingFlag = 0;

		dst->transparent = src->transparent;

		gdImagePaletteCopy(dst, src);

		for (uY = 0; uY < src->sy; uY++) {
			dstY = transY(dst, uX, uY);
			for (uX = 0; uX < src->sx; uX++) {
				c = f (src, uX, uY);
				if (!src->trueColor) {
					r = gdImageRed(src,c);
					g = gdImageGreen(src,c);
					b = gdImageBlue(src,c);
					a = gdImageAlpha(src,c);
					c = gdTrueColorAlpha(r, g, b, a);
				}
				dstX = transX(dst, uX, uY);
				if (ignoretransparent && c == dst->transparent) {
					gdImageSetPixel(dst, dstX, dstY, dst->transparent);
				} else {
					gdImageSetPixel(dst, dstX, dstY, c);
				}
			}
		}
		dst->alphaBlendingFlag = old_blendmode;
	}

	return dst;
}

static int rot90x(gdImagePtr dst, int x, int y)
{
	return y;
}

static int rot90y(gdImagePtr dst, int x, int y)
{
	return (dst->sy - x - 1);
}

static int rot180x(gdImagePtr dst, int x, int y)
{
	return (dst->sx - x - 1);
}

static int rot180y(gdImagePtr dst, int x, int y)
{
	return (dst->sy - y - 1);
}

static int rot270x(gdImagePtr dst, int x, int y)
{
	return (dst->sx - y - 1);
}

static int rot270y(gdImagePtr dst, int x, int y)
{
	return x;
}

static int flipHrot90x(gdImagePtr dst, int x, int y)
{
	return (dst->sx - y - 1);
}

static int flipHrot90y(gdImagePtr dst, int x, int y)
{
	return (dst->sy - x - 1);
}

static int flipHrot180x(gdImagePtr dst, int x, int y)
{
	return x;
}

static int flipHrot180y(gdImagePtr dst, int x, int y)
{
	return (dst->sy - y - 1);
}

static int flipHrot270x(gdImagePtr dst, int x, int y)
{
	return y;
}

static int flipHrot270y(gdImagePtr dst, int x, int y)
{
	return x;
}

static int flipVrot90x(gdImagePtr dst, int x, int y)
{
	return y;
}

static int flipVrot90y(gdImagePtr dst, int x, int y)
{
	return x;
}

static int flipVrot180x(gdImagePtr dst, int x, int y)
{
	return (dst->sx - x - 1);
}

static int flipVrot180y(gdImagePtr dst, int x, int y)
{
	return y;
}

static int flipVrot270x(gdImagePtr dst, int x, int y)
{
	return (dst->sx - y - 1);
}

static int flipVrot270y(gdImagePtr dst, int x, int y)
{
	return (dst->sy - x - 1);
}

static int flipHx(gdImagePtr dst, int x, int y)
{
	return (dst->sx - x - 1);
}

static int flipHy(gdImagePtr dst, int x, int y)
{
	return y;
}

static int flipVx(gdImagePtr dst, int x, int y)
{
	return x;
}

static int flipVy(gdImagePtr dst, int x, int y)
{
	return (dst->sy - y - 1);
}
