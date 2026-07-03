#include "vector2d_example.h"

#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
 * Draw one complete planet in its own local coordinate system.  save/restore
 * makes the translation, rotation, colors, line width, and clip temporary, so
 * the caller can draw the next planet without undoing any of them by hand.
 */
static int draw_planet(gdContextPtr ctx, double angle, double distance,
                       double radius, double red, double green, double blue)
{
    if (!gdContextSave(ctx))
        return 0;

    gdContextTranslate(ctx, 450, 350);
    gdContextRotate(ctx, angle);
    gdContextTranslate(ctx, distance, 0);

    /* The orbiting planet and its shadow now use simple local coordinates. */
    gdContextSetSourceRgba(ctx, 0, 0, 0, 0.28);
    gdContextArc(ctx, 7, 9, radius, 0, 2 * M_PI);
    gdContextFill(ctx);

    gdContextSetSourceRgb(ctx, red, green, blue);
    gdContextArc(ctx, 0, 0, radius, 0, 2 * M_PI);
    gdContextFill(ctx);

    /*
     * A nested saved state confines atmospheric bands to the planet.  Both the
     * circular clip and the translucent paint disappear at restore().
     */
    if (!gdContextSave(ctx)) {
        gdContextRestore(ctx);
        return 0;
    }
    gdContextArc(ctx, 0, 0, radius - 2, 0, 2 * M_PI);
    if (!gdContextClip(ctx)) {
        gdContextRestore(ctx);
        gdContextRestore(ctx);
        return 0;
    }
    gdContextRotate(ctx, -angle * 1.7);
    gdContextSetSourceRgba(ctx, 1, 1, 1, 0.22);
    for (int band = -2; band <= 2; band++) {
        gdContextRectangle(ctx, -radius * 1.4, band * radius * 0.42,
                           radius * 2.8, radius * 0.13);
        gdContextFill(ctx);
    }
    if (!gdContextRestore(ctx))
        return 0;

    /* This highlight is not clipped: restore returned to the planet's state. */
    gdContextSetSourceRgba(ctx, 1, 1, 1, 0.58);
    gdContextArc(ctx, -radius * 0.34, -radius * 0.36,
                 radius * 0.18, 0, 2 * M_PI);
    gdContextFill(ctx);

    return gdContextRestore(ctx);
}

int main(void)
{
    const int width = 900;
    const int height = 700;
    gdImagePtr image = vector2d_create_image(width, height,
        gdTrueColorAlpha(8, 12, 32, 0));
    gdContextPtr ctx;
    gdPaintPtr sky;
    unsigned int seed = 0x5a17u;

    if (!image)
        return 1;
    ctx = gdContextCreateForImage(image);
    if (!ctx) {
        gdImageDestroy(image);
        return 1;
    }

    /* Background state: later local changes cannot leak back into it. */
    sky = gdPaintCreateRadial(450, 350, 20, 450, 350, 520);
    if (!sky)
        goto fail;
    gdPaintAddColorStopRgb(sky, 0, 0.12, 0.08, 0.28);
    gdPaintAddColorStopRgb(sky, 0.55, 0.025, 0.035, 0.12);
    gdPaintAddColorStopRgb(sky, 1, 0.008, 0.012, 0.04);
    gdContextSetSource(ctx, sky);
    gdPaintDestroy(sky);
    gdContextPaint(ctx);

    /* Deterministic stars. */
    for (int i = 0; i < 130; i++) {
        seed = seed * 1664525u + 1013904223u;
        double x = 15 + seed % (width - 30);
        seed = seed * 1664525u + 1013904223u;
        double y = 15 + seed % (height - 30);
        double radius = (i % 17 == 0) ? 1.8 : 0.75;
        gdContextSetSourceRgba(ctx, 0.8, 0.9, 1, 0.45 + (i % 4) * 0.13);
        gdContextArc(ctx, x, y, radius, 0, 2 * M_PI);
        gdContextFill(ctx);
    }

    /* Orbit guides share one style in the untouched root state. */
    gdContextSetSourceRgba(ctx, 0.55, 0.68, 1, 0.18);
    gdContextSetLineWidth(ctx, 1.2);
    for (int orbit = 0; orbit < 4; orbit++) {
        gdContextArc(ctx, 450, 350, 105 + orbit * 65, 0, 2 * M_PI);
        gdContextStroke(ctx);
    }

    /* A glowing sun. */
    sky = gdPaintCreateRadial(450, 350, 8, 450, 350, 88);
    if (!sky)
        goto fail;
    gdPaintAddColorStopRgba(sky, 0, 1, 0.96, 0.62, 1);
    gdPaintAddColorStopRgba(sky, 0.42, 1, 0.48, 0.12, 0.92);
    gdPaintAddColorStopRgba(sky, 1, 1, 0.18, 0.04, 0);
    gdContextSetSource(ctx, sky);
    gdPaintDestroy(sky);
    gdContextArc(ctx, 450, 350, 88, 0, 2 * M_PI);
    gdContextFill(ctx);

    /* Each call starts from exactly the same root state. */
    if (!draw_planet(ctx, -0.55, 105, 22, 0.35, 0.72, 1.0) ||
        !draw_planet(ctx,  1.15, 170, 34, 0.95, 0.42, 0.28) ||
        !draw_planet(ctx,  2.85, 235, 27, 0.48, 0.88, 0.58) ||
        !draw_planet(ctx,  5.45, 300, 43, 0.72, 0.52, 0.96))
        goto fail;

    gdContextFlushImage(ctx);
    if (!vector2d_save_png(image, "save_restore.png"))
        goto fail;

    gdContextDestroy(ctx);
    gdImageDestroy(image);
    puts("Saved save_restore.png");
    return 0;

fail:
    gdContextDestroy(ctx);
    gdImageDestroy(image);
    return 1;
}
