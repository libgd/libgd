/**
 * Bug 788 stride not implemented.
 */

#include "gd.h"
#include "gdtest.h"

#include <libheif/heif.h>

int main () {
	FILE *fp;
	gdImagePtr in;
    gdImagePtr dst;
    gdImagePtr diff;
    int size;
    void *data;
    CuTestImageResult result = {0, 0};
    fp = gdTestFileOpen2("heif", "bug788.png");
    in = gdImageCreateFromPng(fp);
    fclose(fp);
    fp = fopen("1.png", "wb");
    gdImagePng(in, fp);
    fclose(fp);
    data = gdImageHeifPtrEx(in, &size, 200, GD_HEIF_CODEC_HEVC, GD_HEIF_CHROMA_444);

    dst = gdImageCreateFromHeifPtr(size, data);
    diff = gdImageCreateTrueColor(gdImageSX(dst), gdImageSY(dst));
    if (gdTestAssertMsg(dst != NULL, "cannot compare with NULL buffer")) {
        gdTestImageDiff(in, dst, diff, &result);
    }
    fp = fopen("2.png", "wb");
    gdImageHeif(dst, fp);
    fclose(fp);
    fp = fopen("3.png", "wb");
    gdImagePng(diff, fp);
    fclose(fp);
    /* colorspace conversion cannot avoid colors differences, even if we use the same format/colorspace for in and out */
    gdTestAssertMsg(result.pixels_changed > 30, "pixels changed: %d\n", result.pixels_changed);
    gdImageDestroy(dst);
    gdImageDestroy(in);
    gdImageDestroy(diff);
    return 0;
}