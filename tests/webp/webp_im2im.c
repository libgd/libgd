/**
 * Basic test for writing and reading WebP
 *
 * We create an image, write it as WebP and read it in again. We do not
 * actually compare the result with the original image, because we're still
 * lacking a meaningful similarity comparison tool. Instead we store both
 * images in the test directory for manual inspection, if SAVE_IMAGES is set.
 */


#include "gd.h"
#include "gdtest.h"


#define SAVE_IMAGES 0


#if SAVE_IMAGES
static void save_webp(gdImagePtr im, const char *name)
{
	char *path;
	FILE *fp;

	path = gdTestFilePath2("webp", name);
	fp = fopen(path, "wb");
	gdImageWebpEx(im, fp, 100);
	fclose(fp);
	gdFree(path);
}
#endif


int main()
{
	gdImagePtr src, dst;
	int r, g, b;
	void *p;
	int size = 0;

	src = gdImageCreateTrueColor(100, 100);
	gdTestAssert(src != NULL);
	r = gdImageColorAllocate(src, 0xFF, 0, 0);
	g = gdImageColorAllocate(src, 0, 0xFF, 0);
	b = gdImageColorAllocate(src, 0, 0, 0xFF);
	gdImageFilledRectangle(src, 0, 0, 99, 99, r);
	gdImageRectangle(src, 20, 20, 79, 79, g);
	gdImageEllipse(src, 75, 25, 30, 20, b);

#if SAVE_IMAGES
	save_webp(src, "webp_im2im_src.webp");
#endif

	p = gdImageWebpPtrEx(src, &size, 100);
	gdTestAssert(p != NULL);
	gdTestAssert(size > 0);

	dst = gdImageCreateFromWebpPtr(size, p);
	gdTestAssert(dst != NULL);

#if SAVE_IMAGES
	save_webp(dst, "webp_im2im_dst.webp");
#endif

	gdFree(p);
	gdImageDestroy(dst);
	gdImageDestroy(src);

	return gdNumFailures();
}
