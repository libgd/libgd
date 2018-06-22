/* See <https://github.com/libgd/libgd/issues/275>. */


#include "gd.h"
#include "gdtest.h"


#ifdef __GNUC__
#define UNUSED __attribute__((__unused__))
#else
#define UNUSED
#endif


/* define callbacks for a non-seekable fake IO context */

static void fakePutC(UNUSED gdIOCtx *ctx, UNUSED int c)
{
	// do nothing
}

static int fakePutBuf(UNUSED gdIOCtx *ctx, UNUSED const void *data, int wanted)
{
	return wanted;
}


int main()
{
	gdImagePtr im;
	int white;
	gdIOCtx ctx;

	/* initialize the fake IO context */
	ctx.getC = NULL;
	ctx.getBuf = NULL;
	ctx.putC = fakePutC;
	ctx.putBuf = fakePutBuf;
	ctx.seek = NULL;
	ctx.tell = NULL;
	ctx.gd_free = NULL;

	/* create an image */
	im = gdImageCreate(10, 10);
	gdImageColorAllocate(im, 0, 0, 0);
	white = gdImageColorAllocate(im, 255, 255, 255);
	gdImageLine(im, 2,2, 7,7, white);

	/* "save" the image as BMP */
	gdImageBmpCtx(im, &ctx, 2);

	gdImageDestroy(im);

	return gdNumFailures();
}
