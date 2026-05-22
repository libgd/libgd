/*
  GH-980 test: prevent signed int overflow in gdImageCopy* functions.
*/

#include <gd.h>
#include <limits.h>
#include <stdio.h>

static void make_images(gdImagePtr *dst_out, gdImagePtr *src_out, int *expected_pixel_out)
{
    gdImagePtr dst = gdImageCreateTrueColor(2, 2);
    gdImagePtr src = gdImageCreateTrueColor(2, 2);

    if (!dst || !src) {
        fprintf(stderr, "Failed to create images\n");
        if (dst) {
            gdImageDestroy(dst);
        }
        if (src) {
            gdImageDestroy(src);
        }
        *dst_out = NULL;
        *src_out = NULL;
        *expected_pixel_out = 0;
        return;
    }

    int blackDst = gdImageColorAllocate(dst, 0, 0, 0);
    int whiteSrc = gdImageColorAllocate(src, 255, 255, 255);

    gdImageFilledRectangle(dst, 0, 0, 1, 1, blackDst);
    gdImageFilledRectangle(src, 0, 0, 1, 1, whiteSrc);

    *expected_pixel_out = gdImageGetPixel(dst, 0, 0);
    *dst_out = dst;
    *src_out = src;
}

static int assert_dst_unchanged(const char *label, gdImagePtr dst, int expected_pixel)
{
    int actual = gdImageGetPixel(dst, 0, 0);
    if (actual != expected_pixel) {
        printf("FAIL %s: dst changed, got %d expected %d\n", label, actual, expected_pixel);
        return 1;
    }
    printf("OK %s\n", label);
    return 0;
}

int main(void)
{
    int failures = 0;

    /*
      Use values that will overflow a 32 bit signed int when adding w or h.
      INT_MAX is typically 0x7fffffff on 32-bit int systems.
    */
    const int nearIntMax = INT_MAX - 5;
    const int w = 10;
    const int h = 10;

    gdImagePtr dst = NULL, src = NULL;
    int expected = 0;

    /* gdImageCopy */
    make_images(&dst, &src, &expected);
    if (!dst || !src) return 2;
    gdImageCopy(dst, src, nearIntMax, 0, 0, 0, w, h);
    failures += assert_dst_unchanged("imagecopy", dst, expected);
    gdImageDestroy(dst);
    gdImageDestroy(src);

    /* gdImageCopyMerge */
    make_images(&dst, &src, &expected);
    if (!dst || !src) return 2;
    gdImageCopyMerge(dst, src, nearIntMax, 0, 0, 0, w, h, 50);
    failures += assert_dst_unchanged("imagecopymerge", dst, expected);
    gdImageDestroy(dst);
    gdImageDestroy(src);

    /* gdImageCopyMergeGray */
    make_images(&dst, &src, &expected);
    if (!dst || !src) return 2;
    gdImageCopyMergeGray(dst, src, nearIntMax, 0, 0, 0, w, h, 50);
    failures += assert_dst_unchanged("imagecopymergegray", dst, expected);
    gdImageDestroy(dst);
    gdImageDestroy(src);

    /* gdImageCopyResized */
    make_images(&dst, &src, &expected);
    if (!dst || !src) return 2;
    gdImageCopyResized(dst, src, nearIntMax, 0, 0, 0, w, h, 1, 1);
    failures += assert_dst_unchanged("imagecopyresized", dst, expected);
    gdImageDestroy(dst);
    gdImageDestroy(src);

    /* gdImageCopyResampled */
    make_images(&dst, &src, &expected);
    if (!dst || !src) return 2;
    gdImageCopyResampled(dst, src, nearIntMax, 0, 0, 0, w, h, 1, 1);
    failures += assert_dst_unchanged("imagecopyresampled", dst, expected);
    gdImageDestroy(dst);
    gdImageDestroy(src);

    /* gdImageCopyRotated */
    make_images(&dst, &src, &expected);
    if (!dst || !src) return 2;
    gdImageCopyRotated(dst, src, nearIntMax, 0, 0, 0, w, h, 45.0);
    failures += assert_dst_unchanged("imagecopyrotated", dst, expected);
    gdImageDestroy(dst);
    gdImageDestroy(src);

    printf("done\n");
    return failures ? 1 : 0;
}