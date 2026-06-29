#include "gd_vector2d.h"
#include <stdio.h>
#include <string.h>

void save_png(gdImagePtr im, const char *filename)
{
    FILE *fp;
    fp = fopen(filename, "wb");
    if (!fp)
    {
        fprintf(stderr, "Can't save png image %s\n", filename);
        return;
    }
    gdImagePng(im, fp);
    fclose(fp);
}

void test_blend_mode(const char *name, gdImageOp op, int draw_source_alpha)
{
    /* Create a truecolor image */
    gdImagePtr im = gdImageCreateTrueColor(200, 200);
    if (!im) return;

    /* Fill with blue background */
    int blue = gdTrueColorAlpha(0, 0, 255, 0);  /* opaque blue */
    for (int y = 0; y < 200; y++)
    {
        for (int x = 0; x < 200; x++)
        {
            im->tpixels[y][x] = blue;
        }
    }

    /* Create context for the image */
    gdContextPtr ctx = gdContextCreateForImage(im);
    if (!ctx)
    {
        gdImageDestroy(im);
        return;
    }

    /* Set blend mode */
    gdContextSetOperator(ctx, op);

    /* Draw a rectangle with red at specified alpha */
    gdContextSetSourceRgba(ctx, 1.0, 0.0, 0.0, draw_source_alpha);
    gdContextRectangle(ctx, 50, 50, 100, 100);
    gdContextFill(ctx);

    /* Flush back to gdImage */
    gdContextFlushImage(ctx);

    /* Save as PNG */
    gdImageSaveAlpha(im, 1);
    char filename[256];
    snprintf(filename, sizeof(filename), "examples/ctx_image_%s.png", name);
    save_png(im, filename);

    gdContextDestroy(ctx);
    gdImageDestroy(im);

    printf("Saved: %s\n", filename);
}

int main()
{
    /* Test all four blend modes with 50% red source */
    test_blend_mode("srcover_50", gdImageOpsSrcOver, 0.5);
    test_blend_mode("src_50", gdImageOpsSrc, 0.5);
    test_blend_mode("dstin_100", gdImageOpsDstIn, 1.0);   /* DstIn needs opaque source to show effect */
    test_blend_mode("dstout_100", gdImageOpsDstOut, 1.0); /* DstOut needs opaque source */

    return 0;
}
