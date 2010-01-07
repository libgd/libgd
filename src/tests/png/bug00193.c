#include <stdlib.h>
#include "gd.h"
#include "gdtest.h"

static gdImagePtr im;

#define TERMINATE(ctx, status) do {				\
		gdFree(ctx);							\
		gdImageDestroy(im);						\
		exit(status);							\
	} while (0)

static void
failPutC(gdIOCtxPtr ctx, int c)
{
	TERMINATE(ctx, 1);
}

static int
failPutBuf(gdIOCtxPtr ctx, const void * data, int size)
{
	TERMINATE(ctx, 1);
	return -1;
}

int main()
{
	gdIOCtxPtr ctx;

	im = gdImageCreate(100, 100);
	ctx = (gdIOCtxPtr)gdMalloc(sizeof(gdIOCtx));
	ctx->putC   = failPutC;
	ctx->putBuf = failPutBuf;
	gdImagePngCtx(im, ctx);
	TERMINATE(ctx, 0);
	return 0;
}
