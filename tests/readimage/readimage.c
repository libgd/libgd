#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gdtest.h"

#include <stdio.h>
#include <string.h>

static int test_readfile(const char *subdir, const char *filename) {
	char *path;
	gdImagePtr im;

	path = gdTestFilePath2(subdir, filename);
	printf("Testing %s/%s\n", subdir, filename);
	printf("  path: %s\n", path);
	im = gdImageReadFile(path);
	free(path);

	if (im == NULL) {
		gdTestErrorMsg("gdImageReadFile failed for %s/%s\n", subdir, filename);
		return 0;
	}

	gdImageDestroy(im);
	return 1;
}

static int test_readctx(const char *subdir, const char *filename) {
	char *path;
	FILE *fh;
	gdIOCtxPtr ctx;
	gdImagePtr im;

	path = gdTestFilePath2(subdir, filename);
	fh = fopen(path, "rb");
	free(path);

	if (!fh) {
		gdTestErrorMsg("failed to open %s/%s\n", subdir, filename);
		return 0;
	}

	ctx = gdNewFileCtx(fh);
	if (!ctx) {
		fclose(fh);
		gdTestErrorMsg("gdNewFileCtx failed for %s/%s\n", subdir, filename);
		return 0;
	}

	im = gdImageReadCtx(ctx);
	ctx->gd_free(ctx);
	fclose(fh);

	if (im == NULL) {
		gdTestErrorMsg("gdImageReadCtx failed for %s/%s\n", subdir, filename);
		return 0;
	}

	gdImageDestroy(im);
	return 1;
}

int main() {
#ifdef HAVE_LIBPNG
	gdTestAssert(test_readfile("png", "bug00088_1.png"));
	gdTestAssert(test_readctx("png", "bug00088_1.png"));
#endif

	gdTestAssert(test_readfile("gif", "bug00060.gif"));
	gdTestAssert(test_readctx("gif", "bug00060.gif"));

#ifdef HAVE_LIBJPEG
	gdTestAssert(test_readfile("jpeg", "conv_test.jpeg"));
	gdTestAssert(test_readctx("jpeg", "conv_test.jpeg"));
#endif

	gdTestAssert(test_readfile("bmp", "bug00450.bmp"));
	gdTestAssert(test_readctx("bmp", "bug00450.bmp"));

#ifdef HAVE_LIBTIFF
	gdTestAssert(test_readfile("tiff", "tiff_read_bw.tiff"));
	gdTestAssert(test_readctx("tiff", "tiff_read_bw.tiff"));
#endif

#ifdef HAVE_LIBWEBP
	gdTestAssert(test_readfile("webp", "webp-conformance/valid/simple-rgb.webp"));
	gdTestAssert(test_readctx("webp", "webp-conformance/valid/simple-rgb.webp"));
#endif

#ifdef HAVE_LIBAVIF
	gdTestAssert(test_readfile("heif", "label.avif"));
#endif

#ifdef HAVE_LIBHEIF
	gdTestAssert(test_readfile("heif", "label.heic"));
#endif

#ifdef HAVE_LIBJXL
	gdTestAssert(test_readfile("jxl", "basic.jxl"));
	gdTestAssert(test_readctx("jxl", "basic.jxl"));
#endif

	gdTestAssert(test_readfile("qoi", "conformance/qoi_logo.qoi"));
	gdTestAssert(test_readctx("qoi", "conformance/qoi_logo.qoi"));

#ifdef HAVE_LIBXPM
	gdTestAssert(test_readfile("gdimagefile", "img-ref.xpm"));
#endif

	gdTestAssert(test_readfile("xbm", "x10_basic_read.xbm"));

	gdTestAssert(gdImageReadFile(NULL) == NULL);
	gdTestAssert(gdImageReadCtx(NULL) == NULL);

	return gdNumFailures();
}
