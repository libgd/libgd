#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <zlib.h>
#include "gd.h"

extern int gdCosT[1024];
extern int gdSinT[1024];

static void gdImageBrushApply(gdImagePtr im, int x, int y);
static void gdImageTileApply(gdImagePtr im, int x, int y);

gdImagePtr gdImageCreate(int sx, int sy)
{
	int i;
	gdImagePtr im;
	im = (gdImage *) malloc(sizeof(gdImage));
	/* NOW ROW-MAJOR IN GD 1.3 */
	im->pixels = (unsigned char **) malloc(sizeof(unsigned char *) * sy);
	im->polyInts = 0;
	im->polyAllocated = 0;
	im->brush = 0;
	im->tile = 0;
	im->style = 0;
	for (i=0; (i<sy); i++) {
		/* NOW ROW-MAJOR IN GD 1.3 */
		im->pixels[i] = (unsigned char *) calloc(
			sx, sizeof(unsigned char));
	}	
	im->sx = sx;
	im->sy = sy;
	im->colorsTotal = 0;
	im->transparent = (-1);
	im->interlace = 0;

        for (i=0; (i < gdMaxColors); i++) {
           im->open[i] = 1;
	   im->red[i] = 0;
           im->green[i] = 0;
           im->blue[i] = 0;
	};

	return im;
}

void gdImageDestroy(gdImagePtr im)
{
	int i;
	for (i=0; (i<im->sy); i++) {
		free(im->pixels[i]);
	}	
	free(im->pixels);
	if (im->polyInts) {
			free(im->polyInts);
	}
	if (im->style) {
		free(im->style);
	}
	free(im);
}

int gdImageColorClosest(gdImagePtr im, int r, int g, int b)
{
	int i;
	long rd, gd, bd;
	int ct = (-1);
	int first = 1;
	long mindist = 0;
	for (i=0; (i<(im->colorsTotal)); i++) {
		long dist;
		if (im->open[i]) {
			continue;
		}
		rd = (im->red[i] - r);	
		gd = (im->green[i] - g);
		bd = (im->blue[i] - b);
		dist = rd * rd + gd * gd + bd * bd;
		if (first || (dist < mindist)) {
			mindist = dist;	
			ct = i;
			first = 0;
		}
	}
	return ct;
}

int gdImageColorExact(gdImagePtr im, int r, int g, int b)
{
	int i;
	for (i=0; (i<(im->colorsTotal)); i++) {
		if (im->open[i]) {
			continue;
		}
		if ((im->red[i] == r) && 
			(im->green[i] == g) &&
			(im->blue[i] == b)) {
			return i;
		}
	}
	return -1;
}

int gdImageColorAllocate(gdImagePtr im, int r, int g, int b)
{
	int i;
	int ct = (-1);
	for (i=0; (i<(im->colorsTotal)); i++) {
		if (im->open[i]) {
			ct = i;
			break;
		}
	}	
	if (ct == (-1)) {
		ct = im->colorsTotal;
		if (ct == gdMaxColors) {
			return -1;
		}
		im->colorsTotal++;
	}
	im->red[ct] = r;
	im->green[ct] = g;
	im->blue[ct] = b;
	im->open[ct] = 0;
	return ct;
}

/*
 * gdImageColorResolve is an alternative for the code fragment:
 *
 *      if ((color=gdImageColorExact(im,R,G,B)) < 0)
 *        if ((color=gdImageColorAllocate(im,R,G,B)) < 0)
 *          color=gdImageColorClosest(im,R,G,B);
 *
 * in a single function.    Its advantage is that it is guaranteed to
 * return a color index in one search over the color table.
 */
int gdImageColorResolve(gdImagePtr im, int r, int g, int b)
{
        int c;
        int ct = -1;
        int op = -1;
        long rd, gd, bd, dist;
        long mindist = 3*255*255;  /* init to max poss dist */

        for (c = 0; c < im->colorsTotal; c++) {
                if (im->open[c]) {
                        op = c;                         /* Save open slot */
                        continue;                       /* Color not in use */
                }
                rd = (long)(im->red  [c] - r);
                gd = (long)(im->green[c] - g);
                bd = (long)(im->blue [c] - b);
                dist = rd * rd + gd * gd + bd * bd;
                if (dist < mindist) {
                        if (dist == 0) {
                                return c;               /* Return exact match color */
                        }
                        mindist = dist;
                        ct = c;
                }
        }
        /* no exact match.  We now know closest, but first try to allocate exact */
        if (op == -1) {
                op = im->colorsTotal;
                if (op == gdMaxColors) {    /* No room for more colors */
                        return ct;              /* Return closest available color */
                }
                im->colorsTotal++;
        }
        im->red  [op] = r;
        im->green[op] = g;
        im->blue [op] = b;
        im->open [op] = 0;
        return op;                                      /* Return newly allocated color */
}

void gdImageColorDeallocate(gdImagePtr im, int color)
{
	/* Mark it open. */
	im->open[color] = 1;
}

void gdImageColorTransparent(gdImagePtr im, int color)
{
	im->transparent = color;
}

void gdImagePaletteCopy(gdImagePtr to, gdImagePtr from)
{
        int i;
	int x, y, p;
	int xlate[256];

	for (i=0; i < 256 ; i++) {
		xlate[i] = -1;
	};

	for (x=0 ; x < (to->sx) ; x++) {
		for (y=0 ; y < (to->sy) ; y++) {
			p = gdImageGetPixel(to, x, y);
			if (xlate[p] == -1) {
				xlate[p] = gdImageColorClosest(from, to->red[p], to->green[p], to->blue[p]);
				/*printf("Mapping %d (%d, %d, %d) to %d (%d, %d, %d)\n", */
				/*	p,  to->red[p], to->green[p], to->blue[p], */
				/*	xlate[p], from->red[xlate[p]], from->green[xlate[p]], from->blue[xlate[p]]); */
			};
			gdImageSetPixel(to, x, y, xlate[p]);
		};
	};

        for (i=0; (i < (from->colorsTotal) ) ; i++) {
		/*printf("Copying color %d (%d, %d, %d)\n", i, from->red[i], from->blue[i], from->green[i]); */
		to->red[i] = from->red[i];
                to->blue[i] = from->blue[i];
                to->green[i] = from->green[i];
		to->open[i] = 0;
        };

	for (i=from->colorsTotal ; (i < to->colorsTotal) ; i++) { 
		to->open[i] = 1;
	};

	to->colorsTotal = from->colorsTotal;

}

void gdImageSetPixel(gdImagePtr im, int x, int y, int color)
{
	int p;
	switch(color) {
		case gdStyled:
		if (!im->style) {
			/* Refuse to draw if no style is set. */
			return;
		} else {
			p = im->style[im->stylePos++];
		}
		if (p != (gdTransparent)) {
			gdImageSetPixel(im, x, y, p);
		}
		im->stylePos = im->stylePos %  im->styleLength;
		break;
		case gdStyledBrushed:
		if (!im->style) {
			/* Refuse to draw if no style is set. */
			return;
		}
		p = im->style[im->stylePos++];
		if ((p != gdTransparent) && (p != 0)) {
			gdImageSetPixel(im, x, y, gdBrushed);
		}
		im->stylePos = im->stylePos %  im->styleLength;
		break;
		case gdBrushed:
		gdImageBrushApply(im, x, y);
		break;
		case gdTiled:
		gdImageTileApply(im, x, y);
		break;
		default:
		if (gdImageBoundsSafe(im, x, y)) {
			/* NOW ROW-MAJOR IN GD 1.3 */
			im->pixels[y][x] = color;
		}
		break;
	}
}

static void gdImageBrushApply(gdImagePtr im, int x, int y)
{
	int lx, ly;
	int hy;
	int hx;
	int x1, y1, x2, y2;
	int srcx, srcy;
	if (!im->brush) {
		return;
	}
	hy = gdImageSY(im->brush)/2;
	y1 = y - hy;
	y2 = y1 + gdImageSY(im->brush);	
	hx = gdImageSX(im->brush)/2;
	x1 = x - hx;
	x2 = x1 + gdImageSX(im->brush);
	srcy = 0;
	for (ly = y1; (ly < y2); ly++) {
		srcx = 0;
		for (lx = x1; (lx < x2); lx++) {
			int p;
			p = gdImageGetPixel(im->brush, srcx, srcy);
			/* Allow for non-square brushes! */
			if (p != gdImageGetTransparent(im->brush)) {
				gdImageSetPixel(im, lx, ly,
					im->brushColorMap[p]);
			}
			srcx++;
		}
		srcy++;
	}	
}		

static void gdImageTileApply(gdImagePtr im, int x, int y)
{
	int srcx, srcy;
	int p;
	if (!im->tile) {
		return;
	}
	srcx = x % gdImageSX(im->tile);
	srcy = y % gdImageSY(im->tile);
	p = gdImageGetPixel(im->tile, srcx, srcy);
	/* Allow for transparency */
	if (p != gdImageGetTransparent(im->tile)) {
		gdImageSetPixel(im, x, y,
			im->tileColorMap[p]);
	}
}		

int gdImageGetPixel(gdImagePtr im, int x, int y)
{
	if (gdImageBoundsSafe(im, x, y)) {
		/* NOW ROW-MAJOR IN GD 1.3 */
		return im->pixels[y][x];
	} else {
		return 0;
	}
}

/* Bresenham as presented in Foley & Van Dam */

void gdImageLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
	int dx, dy, incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
	dx = abs(x2-x1);
	dy = abs(y2-y1);
	if (dy <= dx) {
		d = 2*dy - dx;
		incr1 = 2*dy;
		incr2 = 2 * (dy - dx);
		if (x1 > x2) {
			x = x2;
			y = y2;
			ydirflag = (-1);
			xend = x1;
		} else {
			x = x1;
			y = y1;
			ydirflag = 1;
			xend = x2;
		}
		gdImageSetPixel(im, x, y, color);
		if (((y2 - y1) * ydirflag) > 0) {
			while (x < xend) {
				x++;
				if (d <0) {
					d+=incr1;
				} else {
					y++;
					d+=incr2;
				}
				gdImageSetPixel(im, x, y, color);
			}
		} else {
			while (x < xend) {
				x++;
				if (d <0) {
					d+=incr1;
				} else {
					y--;
					d+=incr2;
				}
				gdImageSetPixel(im, x, y, color);
			}
		}		
	} else {
		d = 2*dx - dy;
		incr1 = 2*dx;
		incr2 = 2 * (dx - dy);
		if (y1 > y2) {
			y = y2;
			x = x2;
			yend = y1;
			xdirflag = (-1);
		} else {
			y = y1;
			x = x1;
			yend = y2;
			xdirflag = 1;
		}
		gdImageSetPixel(im, x, y, color);
		if (((x2 - x1) * xdirflag) > 0) {
			while (y < yend) {
				y++;
				if (d <0) {
					d+=incr1;
				} else {
					x++;
					d+=incr2;
				}
				gdImageSetPixel(im, x, y, color);
			}
		} else {
			while (y < yend) {
				y++;
				if (d <0) {
					d+=incr1;
				} else {
					x--;
					d+=incr2;
				}
				gdImageSetPixel(im, x, y, color);
			}
		}
	}
}

static void dashedSet(gdImagePtr im, int x, int y, int color,
	int *onP, int *dashStepP);

void gdImageDashedLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
	int dx, dy, incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
	int dashStep = 0;
	int on = 1;
	dx = abs(x2-x1);
	dy = abs(y2-y1);
	if (dy <= dx) {
		d = 2*dy - dx;
		incr1 = 2*dy;
		incr2 = 2 * (dy - dx);
		if (x1 > x2) {
			x = x2;
			y = y2;
			ydirflag = (-1);
			xend = x1;
		} else {
			x = x1;
			y = y1;
			ydirflag = 1;
			xend = x2;
		}
		dashedSet(im, x, y, color, &on, &dashStep);
		if (((y2 - y1) * ydirflag) > 0) {
			while (x < xend) {
				x++;
				if (d <0) {
					d+=incr1;
				} else {
					y++;
					d+=incr2;
				}
				dashedSet(im, x, y, color, &on, &dashStep);
			}
		} else {
			while (x < xend) {
				x++;
				if (d <0) {
					d+=incr1;
				} else {
					y--;
					d+=incr2;
				}
				dashedSet(im, x, y, color, &on, &dashStep);
			}
		}		
	} else {
		d = 2*dx - dy;
		incr1 = 2*dx;
		incr2 = 2 * (dx - dy);
		if (y1 > y2) {
			y = y2;
			x = x2;
			yend = y1;
			xdirflag = (-1);
		} else {
			y = y1;
			x = x1;
			yend = y2;
			xdirflag = 1;
		}
		dashedSet(im, x, y, color, &on, &dashStep);
		if (((x2 - x1) * xdirflag) > 0) {
			while (y < yend) {
				y++;
				if (d <0) {
					d+=incr1;
				} else {
					x++;
					d+=incr2;
				}
				dashedSet(im, x, y, color, &on, &dashStep);
			}
		} else {
			while (y < yend) {
				y++;
				if (d <0) {
					d+=incr1;
				} else {
					x--;
					d+=incr2;
				}
				dashedSet(im, x, y, color, &on, &dashStep);
			}
		}
	}
}

static void dashedSet(gdImagePtr im, int x, int y, int color,
	int *onP, int *dashStepP)
{
	int dashStep = *dashStepP;
	int on = *onP;
	dashStep++;
	if (dashStep == gdDashSize) {
		dashStep = 0;
		on = !on;
	}
	if (on) {
		gdImageSetPixel(im, x, y, color);
	}
	*dashStepP = dashStep;
	*onP = on;
}
	

int gdImageBoundsSafe(gdImagePtr im, int x, int y)
{
	return (!(((y < 0) || (y >= im->sy)) ||
		((x < 0) || (x >= im->sx))));
}

void gdImageChar(gdImagePtr im, gdFontPtr f, int x, int y, 
	int c, int color)
{
	int cx, cy;
	int px, py;
	int fline;
	cx = 0;
	cy = 0;
	if ((c < f->offset) || (c >= (f->offset + f->nchars))) {
		return;
	}
	fline = (c - f->offset) * f->h * f->w;
	for (py = y; (py < (y + f->h)); py++) {
		for (px = x; (px < (x + f->w)); px++) {
			if (f->data[fline + cy * f->w + cx]) {
				gdImageSetPixel(im, px, py, color);	
			}
			cx++;
		}
		cx = 0;
		cy++;
	}
}

void gdImageCharUp(gdImagePtr im, gdFontPtr f, 
	int x, int y, int c, int color)
{
	int cx, cy;
	int px, py;
	int fline;
	cx = 0;
	cy = 0;
	if ((c < f->offset) || (c >= (f->offset + f->nchars))) {
		return;
	}
	fline = (c - f->offset) * f->h * f->w;
	for (py = y; (py > (y - f->w)); py--) {
		for (px = x; (px < (x + f->h)); px++) {
			if (f->data[fline + cy * f->w + cx]) {
				gdImageSetPixel(im, px, py, color);	
			}
			cy++;
		}
		cy = 0;
		cx++;
	}
}

void gdImageString(gdImagePtr im, gdFontPtr f, 
	int x, int y, unsigned char *s, int color)
{
	int i;
	int l;
	l = strlen((char *) s);
	for (i=0; (i<l); i++) {
		gdImageChar(im, f, x, y, s[i], color);
		x += f->w;
	}
}

void gdImageStringUp(gdImagePtr im, gdFontPtr f, 
	int x, int y, unsigned char *s, int color)
{
	int i;
	int l;
	l = strlen((char *) s);
	for (i=0; (i<l); i++) {
		gdImageCharUp(im, f, x, y, s[i], color);
		y -= f->w;
	}
}

static int strlen16(unsigned short *s);

void gdImageString16(gdImagePtr im, gdFontPtr f, 
	int x, int y, unsigned short *s, int color)
{
	int i;
	int l;
	l = strlen16(s);
	for (i=0; (i<l); i++) {
		gdImageChar(im, f, x, y, s[i], color);
		x += f->w;
	}
}

void gdImageStringUp16(gdImagePtr im, gdFontPtr f, 
	int x, int y, unsigned short *s, int color)
{
	int i;
	int l;
	l = strlen16(s);
	for (i=0; (i<l); i++) {
		gdImageCharUp(im, f, x, y, s[i], color);
		y -= f->w;
	}
}

static int strlen16(unsigned short *s)
{
	int len = 0;
	while (*s) {
		s++;
		len++;
	}
	return len;
}

/* s and e are integers modulo 360 (degrees), with 0 degrees
  being the rightmost extreme and degrees changing clockwise.
  cx and cy are the center in pixels; w and h are the horizontal 
  and vertical diameter in pixels. Nice interface, but slow, since
  I don't yet use Bresenham (I'm using an inefficient but
  simple solution with too much work going on in it; generalizing
  Bresenham to ellipses and partial arcs of ellipses is non-trivial,
  at least for me) and there are other inefficiencies (small circles
  do far too much work). */

void gdImageArc(gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color)
{
	int i;
	int lx = 0, ly = 0;
	int w2, h2;
	w2 = w/2;
	h2 = h/2;
	while (e < s) {
		e += 360;
	}
	for (i=s; (i <= e); i++) {
		int x, y;
		x = ((long)gdCosT[i % 360] * (long)w2 / 1024) + cx; 
		y = ((long)gdSinT[i % 360] * (long)h2 / 1024) + cy;
		if (i != s) {
			gdImageLine(im, lx, ly, x, y, color);	
		}
		lx = x;
		ly = y;
	}
}


#if 0
	/* Bresenham octant code, which I should use eventually */
	int x, y, d;
	x = 0;
	y = w;
	d = 3-2*w;
	while (x < y) {
		gdImageSetPixel(im, cx+x, cy+y, color);
		if (d < 0) {
			d += 4 * x + 6;
		} else {
			d += 4 * (x - y) + 10;
			y--;
		}
		x++;
	}
	if (x == y) {
		gdImageSetPixel(im, cx+x, cy+y, color);
	}
#endif

void gdImageFillToBorder(gdImagePtr im, int x, int y, int border, int color)
{
	int lastBorder;
	/* Seek left */
	int leftLimit, rightLimit;
	int i;
	leftLimit = (-1);
	if (border < 0) {
		/* Refuse to fill to a non-solid border */
		return;
	}
	for (i = x; (i >= 0); i--) {
		if (gdImageGetPixel(im, i, y) == border) {
			break;
		}
		gdImageSetPixel(im, i, y, color);
		leftLimit = i;
	}
	if (leftLimit == (-1)) {
		return;
	}
	/* Seek right */
	rightLimit = x;
	for (i = (x+1); (i < im->sx); i++) {	
		if (gdImageGetPixel(im, i, y) == border) {
			break;
		}
		gdImageSetPixel(im, i, y, color);
		rightLimit = i;
	}
	/* Look at lines above and below and start paints */
	/* Above */
	if (y > 0) {
		lastBorder = 1;
		for (i = leftLimit; (i <= rightLimit); i++) {
			int c;
			c = gdImageGetPixel(im, i, y-1);
			if (lastBorder) {
				if ((c != border) && (c != color)) {	
					gdImageFillToBorder(im, i, y-1, 
						border, color);		
					lastBorder = 0;
				}
			} else if ((c == border) || (c == color)) {
				lastBorder = 1;
			}
		}
	}
	/* Below */
	if (y < ((im->sy) - 1)) {
		lastBorder = 1;
		for (i = leftLimit; (i <= rightLimit); i++) {
			int c;
			c = gdImageGetPixel(im, i, y+1);
			if (lastBorder) {
				if ((c != border) && (c != color)) {	
					gdImageFillToBorder(im, i, y+1, 
						border, color);		
					lastBorder = 0;
				}
			} else if ((c == border) || (c == color)) {
				lastBorder = 1;
			}
		}
	}
}

void gdImageFill(gdImagePtr im, int x, int y, int color)
{
	int lastBorder;
	int old;
	int leftLimit, rightLimit;
	int i;
	old = gdImageGetPixel(im, x, y);
	if (color == gdTiled) {
		/* Tile fill -- got to watch out! */
		int p, tileColor;	
		int srcx, srcy;
		if (!im->tile) {
			return;
		}
		/* Refuse to flood-fill with a transparent pattern --
			I can't do it without allocating another image */
		if (gdImageGetTransparent(im->tile) != (-1)) {
			return;
		}	
		srcx = x % gdImageSX(im->tile);
		srcy = y % gdImageSY(im->tile);
		p = gdImageGetPixel(im->tile, srcx, srcy);
		tileColor = im->tileColorMap[p];
		if (old == tileColor) {
			/* Nothing to be done */
			return;
		}
	} else {
		if (old == color) {
			/* Nothing to be done */
			return;
		}
	}
	/* Seek left */
	leftLimit = (-1);
	for (i = x; (i >= 0); i--) {
		if (gdImageGetPixel(im, i, y) != old) {
			break;
		}
		gdImageSetPixel(im, i, y, color);
		leftLimit = i;
	}
	if (leftLimit == (-1)) {
		return;
	}
	/* Seek right */
	rightLimit = x;
	for (i = (x+1); (i < im->sx); i++) {	
		if (gdImageGetPixel(im, i, y) != old) {
			break;
		}
		gdImageSetPixel(im, i, y, color);
		rightLimit = i;
	}
	/* Look at lines above and below and start paints */
	/* Above */
	if (y > 0) {
		lastBorder = 1;
		for (i = leftLimit; (i <= rightLimit); i++) {
			int c;
			c = gdImageGetPixel(im, i, y-1);
			if (lastBorder) {
				if (c == old) {	
					gdImageFill(im, i, y-1, color);		
					lastBorder = 0;
				}
			} else if (c != old) {
				lastBorder = 1;
			}
		}
	}
	/* Below */
	if (y < ((im->sy) - 1)) {
		lastBorder = 1;
		for (i = leftLimit; (i <= rightLimit); i++) {
			int c;
			c = gdImageGetPixel(im, i, y+1);
			if (lastBorder) {
				if (c == old) {
					gdImageFill(im, i, y+1, color);		
					lastBorder = 0;
				}
			} else if (c != old) {
				lastBorder = 1;
			}
		}
	}
}

void gdImageRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
	gdImageLine(im, x1, y1, x2, y1, color);		
	gdImageLine(im, x1, y2, x2, y2, color);		
	gdImageLine(im, x1, y1, x1, y2, color);
	gdImageLine(im, x2, y1, x2, y2, color);
}

void gdImageFilledRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
	int x, y;
	for (y=y1; (y<=y2); y++) {
		for (x=x1; (x<=x2); x++) {
			gdImageSetPixel(im, x, y, color);
		}
	}
}

void gdImageCopy(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h)
{
	int c;
	int x, y;
	int tox, toy;
	int i;
	int colorMap[gdMaxColors];
	for (i=0; (i<gdMaxColors); i++) {
		colorMap[i] = (-1);
	}
	toy = dstY;
	for (y=srcY; (y < (srcY + h)); y++) {
		tox = dstX;
		for (x=srcX; (x < (srcX + w)); x++) {
			int nc;
			c = gdImageGetPixel(src, x, y);
			/* Added 7/24/95: support transparent copies */
			if (gdImageGetTransparent(src) == c) {
				tox++;
				continue;
			}
			/* Have we established a mapping for this color? */
			if (colorMap[c] == (-1)) {
				/* If it's the same image, mapping is trivial */
				if (dst == src) {
					nc = c;
				} else { 
					/* First look for an exact match */
					nc = gdImageColorExact(dst,
						src->red[c], src->green[c],
						src->blue[c]);
				}	
				if (nc == (-1)) {
					/* No, so try to allocate it */
					nc = gdImageColorAllocate(dst,
						src->red[c], src->green[c],
						src->blue[c]);
					/* If we're out of colors, go for the
						closest color */
					if (nc == (-1)) {
						nc = gdImageColorClosest(dst,
							src->red[c], src->green[c],
							src->blue[c]);
					}
				}
				colorMap[c] = nc;
			}
			gdImageSetPixel(dst, tox, toy, colorMap[c]);
			tox++;
		}
		toy++;
	}
}			

void gdImageCopyMerge(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h, int pct)
{
 
        int c, dc;
        int x, y;
        int tox, toy;
        int i;
	int ncR, ncG, ncB;
        int colorMap[gdMaxColors];
        for (i=0; (i<gdMaxColors); i++) {
                colorMap[i] = (-1);
        }
        toy = dstY;
        for (y=srcY; (y < (srcY + h)); y++) {
                tox = dstX;
                for (x=srcX; (x < (srcX + w)); x++) {
                        int nc;
                        c = gdImageGetPixel(src, x, y);
                        /* Added 7/24/95: support transparent copies */
                        if (gdImageGetTransparent(src) == c) {
                                tox++;
                                continue;
                        }
                        /* Have we established a mapping for this color? */
                        /*if (colorMap[c] == (-1)) { */
                                /* If it's the same image, mapping is trivial */
                                if (dst == src) {
                                        nc = c;
                                } else {
					dc = gdImageGetPixel(dst, tox, toy);

					ncR = src->red[c] * (pct/100.0) 
						+ dst->red[dc] * ((100-pct)/100.0);
                                        ncG = src->green[c] * (pct/100.0)
                                                + dst->green[dc] * ((100-pct)/100.0);
                                        ncB = src->blue[c] * (pct/100.0)
                                                + dst->blue[dc] * ((100-pct)/100.0);

                                        /* First look for an exact match */
                                        nc = gdImageColorExact(dst,ncR, ncG, ncB);
                                	if (nc == (-1)) {
                                        	/* No, so try to allocate it */
                                        	nc = gdImageColorAllocate(dst, ncR, ncG, ncB);
                                        	/* If we're out of colors, go for the
                                                	closest color */
                                        	if (nc == (-1)) {
                                                	nc = gdImageColorClosest(dst, ncR, ncG, ncB);
                                        	}
					}
                                }
                                /*colorMap[c] = nc; */
                        /*} */
                        gdImageSetPixel(dst, tox, toy, nc);
                        tox++;
                }
                toy++;
        }
}

void gdImageCopyMergeGray(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h, int pct)
{
 
        int c, dc;
        int x, y;
        int tox, toy;
        int i;
	int ncR, ncG, ncB;
        int colorMap[gdMaxColors];
	float g;

        for (i=0; (i<gdMaxColors); i++) {
                colorMap[i] = (-1);
        }
        toy = dstY;
        for (y=srcY; (y < (srcY + h)); y++) {
                tox = dstX;
                for (x=srcX; (x < (srcX + w)); x++) {
                        int nc;
                        c = gdImageGetPixel(src, x, y);
                        /* Added 7/24/95: support transparent copies */
                        if (gdImageGetTransparent(src) == c) {
                                tox++;
                                continue;
                        }


			dc = gdImageGetPixel(dst, tox, toy);
			g = 0.29900*dst->red[dc] 
				+ 0.58700*dst->green[dc]
				+ 0.11400*dst->blue[dc];

			ncR = src->red[c] * (pct/100.0) 
				+ g * ((100-pct)/100.0);
			ncG = src->green[c] * (pct/100.0)
				+ g * ((100-pct)/100.0);
			ncB = src->blue[c] * (pct/100.0)
				+ g * ((100-pct)/100.0);

			/* First look for an exact match */
			nc = gdImageColorExact(dst,ncR, ncG, ncB);
			if (nc == (-1)) {
				/* No, so try to allocate it */
				nc = gdImageColorAllocate(dst, ncR, ncG, ncB);
				/* If we're out of colors, go for the
					closest color */
				if (nc == (-1)) {
					nc = gdImageColorClosest(dst, ncR, ncG, ncB);
				}
			}
                        gdImageSetPixel(dst, tox, toy, nc);
                        tox++;
                }
                toy++;
        }
}


void gdImageCopyResized(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH)
{
	int c;
	int x, y;
	int tox, toy;
	int ydest;
	int i;
	int colorMap[gdMaxColors];
	/* Stretch vectors */
	int *stx;
	int *sty;
	/* We only need to use floating point to determine the correct
		stretch vector for one line's worth. */
	double accum;
	stx = (int *) malloc(sizeof(int) * srcW);
	sty = (int *) malloc(sizeof(int) * srcH);
	accum = 0;
	for (i=0; (i < srcW); i++) {
		int got;
		accum += (double)dstW/(double)srcW;
		got = (int) floor(accum);
		stx[i] = got;
		accum -= got;
	}
	accum = 0;
	for (i=0; (i < srcH); i++) {
		int got;
		accum += (double)dstH/(double)srcH;
		got = (int) floor(accum);
		sty[i] = got;
		accum -= got;
	}
	for (i=0; (i<gdMaxColors); i++) {
		colorMap[i] = (-1);
	}
	toy = dstY;
	for (y=srcY; (y < (srcY + srcH)); y++) {
		for (ydest=0; (ydest < sty[y-srcY]); ydest++) {
			tox = dstX;
			for (x=srcX; (x < (srcX + srcW)); x++) {
				int nc;
				if (!stx[x - srcX]) {
					continue;
				}
				c = gdImageGetPixel(src, x, y);
				/* Added 7/24/95: support transparent copies */
				if (gdImageGetTransparent(src) == c) {
					tox += stx[x-srcX];
					continue;
				}
				/* Have we established a mapping for this color? */
				if (colorMap[c] == (-1)) {
					/* If it's the same image, mapping is trivial */
					if (dst == src) {
						nc = c;
					} else { 
						/* First look for an exact match */
						nc = gdImageColorExact(dst,
							src->red[c], src->green[c],
							src->blue[c]);
					}	
					if (nc == (-1)) {
						/* No, so try to allocate it */
						nc = gdImageColorAllocate(dst,
							src->red[c], src->green[c],
							src->blue[c]);
						/* If we're out of colors, go for the
							closest color */
						if (nc == (-1)) {
							nc = gdImageColorClosest(dst,
								src->red[c], src->green[c],
								src->blue[c]);
						}
					}
					colorMap[c] = nc;
				}
				for (i=0; (i < stx[x - srcX]); i++) {
					gdImageSetPixel(dst, tox, toy, colorMap[c]);
					tox++;
				}
			}
			toy++;
		}
	}
	free(stx);
	free(sty);
}

gdImagePtr
gdImageCreateFromXbm(FILE *fd)
{
	gdImagePtr im;	
	int bit;
	int w, h;
	int bytes;
	int ch;
	int i, x, y;
	char *sp;
	char s[161];
	if (!fgets(s, 160, fd)) {
		return 0;
	}
	sp = &s[0];
	/* Skip #define */
	sp = strchr(sp, ' ');
	if (!sp) {
		return 0;
	}
	/* Skip width label */
	sp++;
	sp = strchr(sp, ' ');
	if (!sp) {
		return 0;
	}
	/* Get width */
	w = atoi(sp + 1);
	if (!w) {
		return 0;
	}
	if (!fgets(s, 160, fd)) {
		return 0;
	}
	sp = s;
	/* Skip #define */
	sp = strchr(sp, ' ');
	if (!sp) {
		return 0;
	}
	/* Skip height label */
	sp++;
	sp = strchr(sp, ' ');
	if (!sp) {
		return 0;
	}
	/* Get height */
	h = atoi(sp + 1);
	if (!h) {
		return 0;
	}
	/* Skip declaration line */
	if (!fgets(s, 160, fd)) {
		return 0;
	}
	bytes = (w * h / 8) + 1;
	im = gdImageCreate(w, h);
	gdImageColorAllocate(im, 255, 255, 255);
	gdImageColorAllocate(im, 0, 0, 0);
	x = 0;
	y = 0;
	for (i=0; (i < bytes); i++) {
		char h[3];
		int b;
		/* Skip spaces, commas, CRs, 0x */
		while(1) {
			ch = getc(fd);
			if (ch == EOF) {
				goto fail;
			}
			if (ch == 'x') {
				break;
			}	
		}
		/* Get hex value */
		ch = getc(fd);
		if (ch == EOF) {
			goto fail;
		}
		h[0] = ch;
		ch = getc(fd);
		if (ch == EOF) {
			goto fail;
		}
		h[1] = ch;
		h[2] = '\0';
		sscanf(h, "%x", &b);		
		for (bit = 1; (bit <= 128); (bit = bit << 1)) {
			gdImageSetPixel(im, x++, y, (b & bit) ? 1 : 0);	
			if (x == im->sx) {
				x = 0;
				y++;
				if (y == im->sy) {
					return im;
				}
				/* Fix 8/8/95 */
				break;
			}
		}
	}
	/* Shouldn't happen */
	fprintf(stderr, "Error: bug in gdImageCreateFromXbm!\n");
	return 0;
fail:
	gdImageDestroy(im);
	return 0;
}

void gdImagePolygon(gdImagePtr im, gdPointPtr p, int n, int c)
{
	int i;
	int lx, ly;
	if (!n) {
		return;
	}
	lx = p->x;
	ly = p->y;
	gdImageLine(im, lx, ly, p[n-1].x, p[n-1].y, c);
	for (i=1; (i < n); i++) {
		p++;
		gdImageLine(im, lx, ly, p->x, p->y, c);
		lx = p->x;
		ly = p->y;
	}
}	
	
int gdCompareInt(const void *a, const void *b);
	
/* THANKS to Kirsten Schulz for the polygon fixes! */

/* The intersection finding technique of this code could be improved  */
/* by remembering the previous intertersection, and by using the slope.*/
/* That could help to adjust intersections  to produce a nice */
/* interior_extrema. */

void gdImageFilledPolygon(gdImagePtr im, gdPointPtr p, int n, int c)
{
	int i;
	int y;
	int miny, maxy;
	int x1, y1;
	int x2, y2;
	int ind1, ind2;
	int ints;
	if (!n) {
		return;
	}
	if (!im->polyAllocated) {
		im->polyInts = (int *) malloc(sizeof(int) * n);
		im->polyAllocated = n;
	}		
	if (im->polyAllocated < n) {
		while (im->polyAllocated < n) {
			im->polyAllocated *= 2;
		}	
		im->polyInts = (int *) realloc(im->polyInts,
			sizeof(int) * im->polyAllocated);
	}
	miny = p[0].y;
	maxy = p[0].y;
	for (i=1; (i < n); i++) {
		if (p[i].y < miny) {
			miny = p[i].y;
		}
		if (p[i].y > maxy) {
			maxy = p[i].y;
		}
	}
	/* Fix in 1.3: count a vertex only once */
	for (y=miny; (y <= maxy); y++) {
/*1.4		int interLast = 0; */
/*		int dirLast = 0; */
/*		int interFirst = 1; */
		ints = 0;
		for (i=0; (i < n); i++) {
			if (!i) {
				ind1 = n-1;
				ind2 = 0;
			} else {
				ind1 = i-1;
				ind2 = i;
			}
			y1 = p[ind1].y;
			y2 = p[ind2].y;
			if (y1 < y2) {
				x1 = p[ind1].x;
				x2 = p[ind2].x;
			} else if (y1 > y2) {
				y2 = p[ind1].y;
				y1 = p[ind2].y;
				x2 = p[ind1].x;
				x1 = p[ind2].x;
			} else {
				continue;
			}
			if ((y >= y1) && (y < y2)) {
				im->polyInts[ints++] = (y-y1) * (x2-x1) / (y2-y1) + x1;
			} else if ((y == maxy) && (y > y1) && (y <= y2)) {
				im->polyInts[ints++] = (y-y1) * (x2-x1) / (y2-y1) + x1;
			}
		}
		qsort(im->polyInts, ints, sizeof(int), gdCompareInt);

		for (i=0; (i < (ints)); i+=2) {
			gdImageLine(im, im->polyInts[i], y,
				im->polyInts[i+1], y, c);
		}
	}
}
	
int gdCompareInt(const void *a, const void *b)
{
	return (*(const int *)a) - (*(const int *)b);
}

void gdImageSetStyle(gdImagePtr im, int *style, int noOfPixels)
{
	if (im->style) {
		free(im->style);
	}
	im->style = (int *) 
		malloc(sizeof(int) * noOfPixels);
	memcpy(im->style, style, sizeof(int) * noOfPixels);
	im->styleLength = noOfPixels;
	im->stylePos = 0;
}

void gdImageSetBrush(gdImagePtr im, gdImagePtr brush)
{
	int i;
	im->brush = brush;
	for (i=0; (i < gdImageColorsTotal(brush)); i++) {
		int index;
		index = gdImageColorExact(im, 
			gdImageRed(brush, i),
			gdImageGreen(brush, i),
			gdImageBlue(brush, i));
		if (index == (-1)) {
			index = gdImageColorAllocate(im,
				gdImageRed(brush, i),
				gdImageGreen(brush, i),
				gdImageBlue(brush, i));
			if (index == (-1)) {
				index = gdImageColorClosest(im,
					gdImageRed(brush, i),
					gdImageGreen(brush, i),
					gdImageBlue(brush, i));
			}
		}
		im->brushColorMap[i] = index;
	}
}
	
void gdImageSetTile(gdImagePtr im, gdImagePtr tile)
{
	int i;
	im->tile = tile;
	for (i=0; (i < gdImageColorsTotal(tile)); i++) {
		int index;
		index = gdImageColorExact(im, 
			gdImageRed(tile, i),
			gdImageGreen(tile, i),
			gdImageBlue(tile, i));
		if (index == (-1)) {
			index = gdImageColorAllocate(im,
				gdImageRed(tile, i),
				gdImageGreen(tile, i),
				gdImageBlue(tile, i));
			if (index == (-1)) {
				index = gdImageColorClosest(im,
					gdImageRed(tile, i),
					gdImageGreen(tile, i),
					gdImageBlue(tile, i));
			}
		}
		im->tileColorMap[i] = index;
	}
}

void gdImageInterlace(gdImagePtr im, int interlaceArg)
{
	im->interlace = interlaceArg;
}

int gdImageCompare(gdImagePtr im1, gdImagePtr im2)
{
	int	x, y;
	int	p1, p2;
	int	cmpStatus = 0;
	int	sx, sy;

	if (im1->interlace != im2->interlace) {
		cmpStatus |= GD_CMP_INTERLACE;
	}

	if (im1->transparent != im2->transparent) {
		cmpStatus |= GD_CMP_TRANSPARENT;
	}

	if (im1->sx != im2->sx) {
		cmpStatus |= GD_CMP_SIZE_X + GD_CMP_IMAGE;
		if (im1->sx < im2->sx) {
			sx = im1->sx;
		} else {
			sx = im2->sx;
		}
	} else {
		sx = im1->sx;
	}

	if (im1->sy != im2->sy) {
		cmpStatus |= GD_CMP_SIZE_Y + GD_CMP_IMAGE;
		if (im1->sy < im2->sy) {
			sy = im1->sy;
		} else {
			sy = im2->sy;
		}
	}

	if (im1->colorsTotal != im2->colorsTotal) {
		cmpStatus |= GD_CMP_NUM_COLORS;
	}

	for ( y = 0 ; (y<im1->sy) ; y++ ) {
		for ( x = 0 ; (x < im1->sx) ; x++ ) {
			p1 = im1->pixels[y][x];
			p2 = im2->pixels[y][x];

			if (im1->red[p1] != im2->red[p2]) {
				cmpStatus |= GD_CMP_COLOR + GD_CMP_IMAGE;
				break;
			}

			if (im1->green[p1] != im2->green[p2]) {
				cmpStatus |= GD_CMP_COLOR + GD_CMP_IMAGE;
				break;
			}


			if (im1->blue[p1] != im2->blue[p2]) {
				cmpStatus |= GD_CMP_COLOR + GD_CMP_IMAGE;	
				break;
			}
		}
		if (cmpStatus & GD_CMP_COLOR) { break; };
	}

	return cmpStatus;
}

