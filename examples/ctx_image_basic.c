#include "gd_vector2d.h"
#include <stdio.h>

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

int main()
{
    /* Create a truecolor image */
    gdImagePtr im = gdImageCreateTrueColor(200, 200);
    if (!im)
    {
        fprintf(stderr, "Failed to create image\n");
        return 1;
    }

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
        fprintf(stderr, "Failed to create context\n");
        gdImageDestroy(im);
        return 1;
    }

    /* Draw a semi-transparent red rectangle (50% alpha) */
    gdContextSetSourceRgba(ctx, 1.0, 0.0, 0.0, 0.5);
    gdContextRectangle(ctx, 50, 50, 100, 100);
    gdContextFill(ctx);

    /* Flush back to gdImage */
    gdContextFlushImage(ctx);

    /* Save as PNG (with alpha channel) */
    gdImageSaveAlpha(im, 1);
    save_png(im, "examples/ctx_image_basic.png");

    gdContextDestroy(ctx);
    gdImageDestroy(im);

    printf("Saved: examples/ctx_image_basic.png\n");
    return 0;
}
