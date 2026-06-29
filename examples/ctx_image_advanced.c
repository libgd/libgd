#include "gd_vector2d.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

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

void test_lines()
{
    gdImagePtr im = gdImageCreateTrueColor(200, 200);
    int white = gdTrueColorAlpha(255, 255, 255, 0);
    for (int y = 0; y < 200; y++)
        for (int x = 0; x < 200; x++)
            im->tpixels[y][x] = white;

    gdContextPtr ctx = gdContextCreateForImage(im);

    /* Draw multiple lines with different widths and colors */
    gdContextSetSourceRgba(ctx, 1.0, 0.0, 0.0, 1.0);  // red
    gdContextSetLineWidth(ctx, 2.0);
    gdContextMoveTo(ctx, 10, 10);
    gdContextLineTo(ctx, 190, 190);
    gdContextStroke(ctx);

    gdContextSetSourceRgba(ctx, 0.0, 1.0, 0.0, 0.5);  // 50% green
    gdContextSetLineWidth(ctx, 5.0);
    gdContextMoveTo(ctx, 190, 10);
    gdContextLineTo(ctx, 10, 190);
    gdContextStroke(ctx);

    gdContextSetSourceRgba(ctx, 0.0, 0.0, 1.0, 0.8);  // 80% blue
    gdContextSetLineWidth(ctx, 3.0);
    gdContextMoveTo(ctx, 100, 10);
    gdContextLineTo(ctx, 100, 190);
    gdContextStroke(ctx);

    gdContextSetSourceRgba(ctx, 1.0, 0.5, 0.0, 1.0);  // orange
    gdContextSetLineWidth(ctx, 2.0);
    gdContextMoveTo(ctx, 10, 100);
    gdContextLineTo(ctx, 190, 100);
    gdContextStroke(ctx);

    gdContextFlushImage(ctx);
    gdImageSaveAlpha(im, 1);
    save_png(im, "examples/ctx_image_lines.png");

    gdContextDestroy(ctx);
    gdImageDestroy(im);
    printf("Saved: examples/ctx_image_lines.png\n");
}

void test_curves()
{
    gdImagePtr im = gdImageCreateTrueColor(200, 200);
    int white = gdTrueColorAlpha(255, 255, 255, 0);
    for (int y = 0; y < 200; y++)
        for (int x = 0; x < 200; x++)
            im->tpixels[y][x] = white;

    gdContextPtr ctx = gdContextCreateForImage(im);

    /* Cubic Bezier curve */
    gdContextSetSourceRgba(ctx, 0.8, 0.2, 0.2, 1.0);  // dark red
    gdContextSetLineWidth(ctx, 3.0);
    gdContextMoveTo(ctx, 20, 180);
    gdContextCurveTo(ctx, 50, 20, 150, 20, 180, 180);
    gdContextStroke(ctx);

    /* Quadratic Bezier curve */
    gdContextSetSourceRgba(ctx, 0.2, 0.8, 0.2, 1.0);  // green
    gdContextSetLineWidth(ctx, 2.0);
    gdContextMoveTo(ctx, 20, 180);
    gdContextQuadTo(ctx, 100, 20, 180, 180);
    gdContextStroke(ctx);

    /* Arc */
    gdContextSetSourceRgba(ctx, 0.2, 0.2, 0.8, 1.0);  // blue
    gdContextSetLineWidth(ctx, 4.0);
    gdContextArc(ctx, 100, 100, 80, 0, M_PI);  // semicircle
    gdContextStroke(ctx);

    /* Filled rectangle with alpha */
    gdContextSetSourceRgba(ctx, 1.0, 0.0, 0.0, 0.3);  // 30% red
    gdContextRectangle(ctx, 60, 60, 80, 80);
    gdContextFill(ctx);

    gdContextFlushImage(ctx);
    gdImageSaveAlpha(im, 1);
    save_png(im, "examples/ctx_image_curves.png");

    gdContextDestroy(ctx);
    gdImageDestroy(im);
    printf("Saved: examples/ctx_image_curves.png\n");
}

void test_transformations()
{
    gdImagePtr im = gdImageCreateTrueColor(200, 200);
    int white = gdTrueColorAlpha(255, 255, 255, 0);
    for (int y = 0; y < 200; y++)
        for (int x = 0; x < 200; x++)
            im->tpixels[y][x] = white;

    gdContextPtr ctx = gdContextCreateForImage(im);

    /* Draw rotated rectangles using matrix push/pop via identity reset */
    gdContextSetSourceRgba(ctx, 0.2, 0.6, 0.8, 0.7);  // 70% cyan
    gdContextSetLineWidth(ctx, 2.0);

    for (int i = 0; i < 12; i++)
    {
        gdContextNewPath(ctx);
        /* Reset to identity then apply transform */
        gdContextSetSourceRgba(ctx, 0.2, 0.6, 0.8, 0.7);
        gdContextTranslate(ctx, 100, 100);
        gdContextRotate(ctx, i * M_PI / 6);
        gdContextRectangle(ctx, -40, -40, 80, 80);
        gdContextStroke(ctx);
        /* Reset transform back to identity for next iteration */
        gdContextSetSourceRgba(ctx, 0.2, 0.6, 0.8, 0.7);
    }

    /* Center circle */
    gdContextSetSourceRgba(ctx, 0.8, 0.2, 0.2, 1.0);  // red
    gdContextSetLineWidth(ctx, 3.0);
    gdContextArc(ctx, 100, 100, 20, 0, 2 * M_PI);
    gdContextStroke(ctx);

    gdContextFlushImage(ctx);
    gdImageSaveAlpha(im, 1);
    save_png(im, "examples/ctx_image_transformations.png");

    gdContextDestroy(ctx);
    gdImageDestroy(im);
    printf("Saved: examples/ctx_image_transformations.png\n");
}

void test_dashed_lines()
{
    gdImagePtr im = gdImageCreateTrueColor(200, 200);
    int white = gdTrueColorAlpha(255, 255, 255, 0);
    for (int y = 0; y < 200; y++)
        for (int x = 0; x < 200; x++)
            im->tpixels[y][x] = white;

    gdContextPtr ctx = gdContextCreateForImage(im);

    double dash1[] = {10.0, 5.0};
    gdContextSetDash(ctx, 0, dash1, 2);
    gdContextSetSourceRgba(ctx, 1.0, 0.0, 0.0, 1.0);
    gdContextSetLineWidth(ctx, 3.0);
    gdContextMoveTo(ctx, 20, 50);
    gdContextLineTo(ctx, 180, 50);
    gdContextStroke(ctx);

    double dash2[] = {20.0, 10.0, 5.0, 10.0};
    gdContextSetDash(ctx, 0, dash2, 4);
    gdContextSetSourceRgba(ctx, 0.0, 0.0, 1.0, 1.0);
    gdContextSetLineWidth(ctx, 3.0);
    gdContextMoveTo(ctx, 20, 100);
    gdContextLineTo(ctx, 180, 100);
    gdContextStroke(ctx);

    double dash3[] = {2.0, 2.0};
    gdContextSetDash(ctx, 0, dash3, 2);
    gdContextSetSourceRgba(ctx, 0.0, 0.7, 0.0, 1.0);
    gdContextSetLineWidth(ctx, 2.0);
    gdContextMoveTo(ctx, 20, 150);
    gdContextLineTo(ctx, 180, 150);
    gdContextStroke(ctx);

    /* Solid line for comparison */
    gdContextSetDash(ctx, 0, NULL, 0);
    gdContextSetSourceRgba(ctx, 0.5, 0.5, 0.5, 1.0);
    gdContextSetLineWidth(ctx, 1.0);
    gdContextMoveTo(ctx, 20, 20);
    gdContextLineTo(ctx, 180, 20);
    gdContextStroke(ctx);

    gdContextFlushImage(ctx);
    gdImageSaveAlpha(im, 1);
    save_png(im, "examples/ctx_image_dashed.png");

    gdContextDestroy(ctx);
    gdImageDestroy(im);
    printf("Saved: examples/ctx_image_dashed.png\n");
}

void test_polygon()
{
    gdImagePtr im = gdImageCreateTrueColor(200, 200);
    int white = gdTrueColorAlpha(255, 255, 255, 0);
    for (int y = 0; y < 200; y++)
        for (int x = 0; x < 200; x++)
            im->tpixels[y][x] = white;

    gdContextPtr ctx = gdContextCreateForImage(im);

    /* Star polygon */
    gdContextSetSourceRgba(ctx, 1.0, 0.8, 0.0, 0.8);  // gold, 80% alpha
    gdContextSetLineWidth(ctx, 2.0);
    gdContextMoveTo(ctx, 100, 30);
    for (int i = 1; i < 10; i++)
    {
        double angle = i * 2 * M_PI / 10 - M_PI / 2;
        double r = (i % 2 == 0) ? 70 : 30;
        gdContextLineTo(ctx, 100 + r * cos(angle), 100 + r * sin(angle));
    }
    gdContextClosePath(ctx);
    gdContextFill(ctx);

    /* Outline */
    gdContextSetSourceRgba(ctx, 0.8, 0.0, 0.0, 1.0);
    gdContextSetLineWidth(ctx, 3.0);
    gdContextMoveTo(ctx, 100, 30);
    for (int i = 1; i < 10; i++)
    {
        double angle = i * 2 * M_PI / 10 - M_PI / 2;
        double r = (i % 2 == 0) ? 70 : 30;
        gdContextLineTo(ctx, 100 + r * cos(angle), 100 + r * sin(angle));
    }
    gdContextClosePath(ctx);
    gdContextStroke(ctx);

    gdContextFlushImage(ctx);
    gdImageSaveAlpha(im, 1);
    save_png(im, "examples/ctx_image_polygon.png");

    gdContextDestroy(ctx);
    gdImageDestroy(im);
    printf("Saved: examples/ctx_image_polygon.png\n");
}

void test_clipping()
{
    gdImagePtr im = gdImageCreateTrueColor(200, 200);
    int white = gdTrueColorAlpha(255, 255, 255, 0);
    for (int y = 0; y < 200; y++)
        for (int x = 0; x < 200; x++)
            im->tpixels[y][x] = white;

    gdContextPtr ctx = gdContextCreateForImage(im);

    /* Set clip rectangle */
    gdContextRectangle(ctx, 50, 50, 100, 100);
    gdContextClip(ctx);

    /* Draw a large circle - should be clipped */
    gdContextSetSourceRgba(ctx, 1.0, 0.0, 0.0, 1.0);
    gdContextArc(ctx, 100, 100, 80, 0, 2 * M_PI);
    gdContextFill(ctx);

    /* Draw lines that cross the clip region */
    gdContextSetSourceRgba(ctx, 0.0, 0.0, 1.0, 1.0);
    gdContextSetLineWidth(ctx, 2.0);
    for (int i = 0; i < 200; i += 20)
    {
        gdContextMoveTo(ctx, i, 0);
        gdContextLineTo(ctx, i, 200);
        gdContextStroke(ctx);
    }

    gdContextFlushImage(ctx);
    gdImageSaveAlpha(im, 1);
    save_png(im, "examples/ctx_image_clipping.png");

    gdContextDestroy(ctx);
    gdImageDestroy(im);
    printf("Saved: examples/ctx_image_clipping.png\n");
}

int main()
{
    test_lines();
    test_curves();
    test_transformations();
    test_dashed_lines();
    test_polygon();
    test_clipping();
    return 0;
}
