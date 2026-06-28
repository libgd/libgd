#include "gd.h"
#include "gdtest.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void fill_image(gdImagePtr image, int color)
{
    for (int y = 0; y < gdImageSY(image); y++)
        for (int x = 0; x < gdImageSX(image); x++)
            gdImageSetPixel(image, x, y, color);
}

static gdImagePtr render_strokes(void)
{
    gdImagePtr image = gdImageCreateTrueColor(192, 128);
    fill_image(image, gdTrueColor(255, 255, 255));
    gdContextPtr context = gdContextCreateForImage(image);
    gdContextSetSourceRgb(context, 0.05, 0.08, 0.12);
    gdContextSetLineWidth(context, 14);

    for (int cap = gdLineCapButt; cap <= gdLineCapSquare; cap++) {
        gdContextSetLineCap(context, (gdLineCap)cap);
        gdContextMoveTo(context, 22 + cap * 32, 18);
        gdContextLineTo(context, 22 + cap * 32, 68);
        gdContextStroke(context);
    }

    for (int join = gdLineJoinMiter; join <= gdLineJoinBevel; join++) {
        gdContextSetLineJoin(context, (gdLineJoin)join);
        gdContextMoveTo(context, 112 + join * 30, 62);
        gdContextLineTo(context, 124 + join * 30, 38);
        gdContextLineTo(context, 136 + join * 30, 62);
        gdContextStroke(context);
    }

    const double dash[] = {9, 4, 2, 4};
    gdContextSetLineWidth(context, 4);
    gdContextSetLineCap(context, gdLineCapButt);
    gdContextSetDash(context, 3, dash, 4);
    gdContextSetSourceRgb(context, 0.8, 0.1, 0.15);
    gdContextMoveTo(context, 12, 105);
    gdContextLineTo(context, 180, 105);
    gdContextStroke(context);

    gdContextFlushImage(context);
    gdContextDestroy(context);
    return image;
}

static void nested_rectangles(gdContextPtr context, double x)
{
    gdContextRectangle(context, x, 12, 58, 58);
    gdContextRectangle(context, x + 14, 26, 30, 30);
}

static gdImagePtr render_fills(void)
{
    gdImagePtr image = gdImageCreateTrueColor(192, 128);
    fill_image(image, gdTrueColor(255, 255, 255));
    gdContextPtr context = gdContextCreateForImage(image);

    gdContextSetSourceRgb(context, 0.1, 0.45, 0.8);
    gdContextSetFillRule(context, gdFillRuleNonZero);
    nested_rectangles(context, 12);
    gdContextFill(context);

    gdContextSetFillRule(context, gdFillRulEvenOdd);
    nested_rectangles(context, 82);
    gdContextFill(context);

    gdContextSetFillRule(context, gdFillRuleNonZero);
    gdContextSetSourceRgba(context, 0.9, 0.25, 0.1, 0.8);
    gdContextMoveTo(context, 12, 112);
    gdContextQuadTo(context, 48, 67, 82, 112);
    gdContextCurveTo(context, 112, 72, 148, 72, 180, 112);
    gdContextLineTo(context, 12, 112);
    gdContextFill(context);

    gdContextFlushImage(context);
    gdContextDestroy(context);
    return image;
}

static gdSurfacePtr checker_surface(void)
{
    gdSurfacePtr surface = gdSurfaceCreate(16, 16, GD_SURFACE_ARGB32);
    unsigned char *data = gdSurfaceGetData(surface);
    for (int y = 0; y < 16; y++) {
        uint32_t *row = (uint32_t *)(data + y * surface->stride);
        for (int x = 0; x < 16; x++)
            row[x] = ((x / 4 + y / 4) & 1) ? 0xfff0b429 : 0xff2457c5;
    }
    return surface;
}

static void render_pattern(gdContextPtr context, gdSurfacePtr checker)
{
    gdContextArc(context, 64, 64, 50, 0, 2 * M_PI);
    gdContextClip(context);
    gdPathPatternPtr pattern = gdPathPatternCreate(checker);
    gdPathPatternSetExtend(pattern, GD_EXTEND_REPEAT);
    gdPathMatrix matrix;
    gdPathMatrixInitScale(&matrix, 0.75, 0.75);
    gdPathPatternSetMatrix(pattern, &matrix);
    gdPaintPtr paint = gdPaintCreateFromPattern(pattern);
    gdContextSetSource(context, paint);
    gdContextPaint(context);
    gdPaintDestroy(paint);
    gdPathPatternDestroy(pattern);
}

static gdImagePtr surface_to_image(gdSurfacePtr surface)
{
    char *path = gdTestTempFile("vector2d-surface.png");
    FILE *file = fopen(path, "wb");
    if (!file) {
        free(path);
        return NULL;
    }
    gdSurfacePng(surface, file);
    fclose(file);
    file = fopen(path, "rb");
    if (!file) {
        free(path);
        return NULL;
    }
    gdImagePtr image = gdImageCreateFromPng(file);
    fclose(file);
    free(path);
    return image;
}

static gdImagePtr render_surface_pattern(gdImagePtr *image_backend)
{
    gdSurfacePtr checker = checker_surface();
    gdSurfacePtr destination = gdSurfaceCreate(128, 128, GD_SURFACE_ARGB32);
    unsigned char *data = gdSurfaceGetData(destination);
    for (int y = 0; y < 128; y++) {
        uint32_t *row = (uint32_t *)(data + y * destination->stride);
        for (int x = 0; x < 128; x++) row[x] = 0xffffffff;
    }
    gdContextPtr surface_context = gdContextCreate(destination);
    render_pattern(surface_context, checker);
    gdContextDestroy(surface_context);
    gdImagePtr surface_image = surface_to_image(destination);

    *image_backend = gdImageCreateTrueColor(128, 128);
    fill_image(*image_backend, gdTrueColor(255, 255, 255));
    gdContextPtr image_context = gdContextCreateForImage(*image_backend);
    render_pattern(image_context, checker);
    gdContextFlushImage(image_context);
    gdContextDestroy(image_context);

    gdSurfaceDestroy(destination);
    gdSurfaceDestroy(checker);
    return surface_image;
}

static gdImagePtr render_blend_modes(void)
{
    const int tile = 48, columns = 5;
    gdImagePtr result = gdImageCreateTrueColor(columns * tile, 6 * tile);
    gdImageAlphaBlending(result, gdEffectReplace);
    fill_image(result, gdTrueColorAlpha(245, 245, 245, 0));

    for (int op = 0; op < GD_OP_COUNT; op++) {
        gdImagePtr cell = gdImageCreateTrueColor(tile, tile);
        gdImageAlphaBlending(cell, gdEffectReplace);
        fill_image(cell, gdTrueColorAlpha(0, 0, 0, gdAlphaTransparent));
        gdContextPtr context = gdContextCreateForImage(cell);
        gdContextSetSourceRgba(context, 0.85, 0.12, 0.08, 0.72);
        gdContextRectangle(context, 5, 8, 27, 27);
        gdContextFill(context);
        gdContextSetOperator(context, (gdCompositeOperator)op);
        gdContextSetSourceRgba(context, 0.05, 0.25, 0.9, 0.58);
        gdContextRectangle(context, 17, 15, 27, 27);
        gdContextFill(context);
        gdContextFlushImage(context);
        gdContextDestroy(context);
        gdImageCopy(result, cell, (op % columns) * tile,
                    (op / columns) * tile, 0, 0, tile, tile);
        gdImageDestroy(cell);
    }
    gdImageSaveAlpha(result, 1);
    return result;
}

static int write_png(const char *directory, const char *name, gdImagePtr image)
{
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", directory, name);
    FILE *file = fopen(path, "wb");
    if (!file) return 0;
    gdImagePng(image, file);
    fclose(file);
    return 1;
}

int main(int argc, char **argv)
{
    gdImagePtr strokes = render_strokes();
    gdImagePtr fills = render_fills();
    gdImagePtr image_backend = NULL;
    gdImagePtr pattern = render_surface_pattern(&image_backend);
    gdImagePtr blend_modes = render_blend_modes();

    if (argc == 3 && strcmp(argv[1], "--generate") == 0) {
        if (!write_png(argv[2], "visual_strokes.png", strokes) ||
            !write_png(argv[2], "visual_fills.png", fills) ||
            !write_png(argv[2], "visual_pattern.png", pattern) ||
            !write_png(argv[2], "visual_blend_modes.png", blend_modes))
            gdTestErrorMsg("could not write vector2d golden images\n");
    } else {
        gdAssertImageEqualsToFile("vector2d/visual_strokes.png", strokes);
        gdAssertImageEqualsToFile("vector2d/visual_fills.png", fills);
        gdAssertImageEqualsToFile("vector2d/visual_pattern.png", pattern);
        gdAssertImageEqualsToFile("vector2d/visual_blend_modes.png", blend_modes);
        gdAssertImageEquals(pattern, image_backend);
    }

    gdImageDestroy(strokes);
    gdImageDestroy(fills);
    gdImageDestroy(pattern);
    gdImageDestroy(image_backend);
    gdImageDestroy(blend_modes);
    return gdNumFailures();
}
