
#include <math.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd_intern.h"

/* 2.03: don't include zlib here or we can't build without PNG */
#include "gd.h"
#include "gdhelpers.h"
#include "gd_color.h"
#include "gd_errors.h"
#include "gd_path.h"
#include "gd_span_rle.h"
#include "gd_draw_blend.h"
#include "gd_path_matrix.h"
#include "gd_path_dash.h"

BGD_DECLARE(void)
gdContextSetSourceRgba(gdContextPtr context, double r, double g, double b, double a)
{
    gdPaintPtr source = gdPaintCreateRgba(r, g, b, a);
    gdContextSetSource(context, source);
    gdPaintDestroy(source);
}

BGD_DECLARE(void)
gdContextSetSourceRgb(gdContextPtr context, double r, double g, double b)
{
    gdPaintPtr source = gdPaintCreateRgba(r, g, b, 1.0);
    gdContextSetSource(context, source);
    gdPaintDestroy(source);
}

BGD_DECLARE(void)
gdContextNewPath(gdContextPtr context)
{
    gdPathClear(context->path);
}


BGD_DECLARE(gdContextPtr)
gdContextCreate(gdSurfacePtr surface)
{
    gdContextPtr context = gdMalloc(sizeof(gdContext));
    if (!context)
    {
        return NULL;
    }
    context->state = gdStateCreate();
    if (!context->state)
    {
        goto failState;
    }
    context->path = gdPathCreate();
    if (!context->path)
    {
        goto failPath;
    }
    context->rle = gdSpanRleCreate();
    if (!context->rle)
    {
        goto failRle;
    }
    context->ref = 1;
    context->surface = gdSurfaceAddRef(surface);
    context->clippath= NULL;
    context->clip.x = 0.0;
    context->clip.y = 0.0;
    context->clip.w = surface->width;
    context->clip.h = surface->height;
    return context;
failRle:
    gdFree(context->path);
failPath:
    gdFree(context->state);
failState:
    gdFree(context);
    return NULL;
}

BGD_DECLARE(void)
gdContextStrokePreserve(gdContextPtr context)
{
    gdStatePtr state = context->state;
    gdSpanRleClear(context->rle);
    gdSpanRleRasterize(context->rle, context->path, &state->matrix, &context->clip, &state->stroke, gdFillRuleNonZero);
    gdSpanRlePathClip(context->rle, state->clippath);
    gdPathBlend(context, context->rle);
}

BGD_DECLARE(void)
gdContextFillPreserve(gdContextPtr context)
{
    gdStatePtr state = context->state;
    gdSpanRleClear(context->rle);
    gdSpanRleRasterize(context->rle, context->path, &state->matrix, &context->clip, NULL, state->winding);
    gdSpanRlePathClip(context->rle, state->clippath);
    gdPathBlend(context, context->rle);
}

BGD_DECLARE(void)
gdContextFill(gdContextPtr context)
{
    gdContextFillPreserve(context);
    gdContextNewPath(context);
}

BGD_DECLARE(void)
gdContextClipPreserve(gdContextPtr context)
{
    unsigned int context_path_size;
    const gdPathPtr currentPath = context->path;
    gdStatePtr state = context->state;
    context_path_size = gdArrayNumElements(&currentPath->elements);

    if (context_path_size == 0)
        return;

    if (state->clippath)
    {
        gdSpanRleClear(context->rle);
        gdSpanRleRasterize(context->rle, context->path, &state->matrix, &context->clip, NULL, state->winding);
        gdSpanRlePathClip(state->clippath, context->rle);
    } else {
        state->clippath = gdSpanRleCreate();
        gdSpanRleRasterize(state->clippath, context->path, &state->matrix, &context->clip, NULL, state->winding);
    }
}


BGD_DECLARE(void)
gdContextClip(gdContextPtr context)
{
    gdContextClipPreserve(context);
    gdContextNewPath(context);
}

BGD_DECLARE(void)
gdContextDestroy(gdContextPtr context)
{
    if (context == NULL)
        return;
    context->ref--;
    if (context->ref == 0)
    {
        gdSurfaceDestroy(context->surface);
        gdPathDestroy(context->path);
        gdStateDestroy(context->state);
        gdSpanRleDestroy(context->clippath);
        gdSpanRleDestroy(context->rle);
        gdFree(context);
    }
}

BGD_DECLARE(void)
gdContextMoveTo(gdContextPtr context, double x, double y)
{
    gdPathMoveTo(context->path, x, y);
}

BGD_DECLARE(void)
gdContextLineTo(gdContextPtr context, double x, double y)
{
    gdPathLineTo(context->path, x, y);
}

BGD_DECLARE(void)
gdContextRelLineTo(gdContextPtr context, double x, double y)
{
    gdPathRelLineTo(context->path, x, y);
}

BGD_DECLARE(void)
gdContextSetLineWidth(gdContextPtr context, double width)
{
    context->state->stroke.width = width;
}

BGD_DECLARE(void) gdContextSetDash(gdContextPtr context, double offset, const double* data, int size)
{
    gdPathDashDestroy(context->state->stroke.dash);
    context->state->stroke.dash = gdPathDashCreate(data, size, offset);
}

BGD_DECLARE(void)
gdContextSetLineCap(gdContextPtr context, gdLineCap cap)
{
    context->state->stroke.cap = cap;
}

BGD_DECLARE(void)
gdContextSetLineJoin(gdContextPtr context, gdLineJoin join)
{
    context->state->stroke.join = join;
}

BGD_DECLARE(void)
gdContextCurveTo(gdContextPtr context, double x1, double y1, double x2, double y2, double x3, double y3)
{
    gdPathCurveTo(context->path, x1, y1, x2, y2, x3, y3);
}

BGD_DECLARE(void)
gdContextArc(gdContextPtr context, double cx, double cy, double r, double a0, double a1)
{
    gdPathAddArc(context->path, cx, cy, r, a0, a1, 0);
}

BGD_DECLARE(void)
gdContextNegativeArc(gdContextPtr context, double cx, double cy, double r, double a0, double a1)
{
    gdPathAddArc(context->path, cx, cy, r, a0, a1, 1);
}

BGD_DECLARE(void)
gdContextClosePath(gdContextPtr context)
{
    if (!context)
        return;
    gdPathClose(context->path);
}

BGD_DECLARE(void)
gdContextScale(gdContextPtr context, double x, double y)
{
    gdPathMatrixScale(&context->state->matrix, x, y);
}

BGD_DECLARE(void)
gdContextTranslate(gdContextPtr context, double x, double y)
{
    gdPathMatrixTranslate(&context->state->matrix, x, y);
}

BGD_DECLARE(void)
gdContextRotate(gdContextPtr context, double radians)
{
    gdPathMatrixRotate(&context->state->matrix, radians);
}

BGD_DECLARE(void)
gdContextTransform(gdContextPtr context, const gdPathMatrixPtr matrix)
{
    gdPathMatrixMultiply(&context->state->matrix, matrix, &context->state->matrix);
}

BGD_DECLARE(void)
gdContextSetFillRule(gdContextPtr context, gdFillRule winding)
{
    context->state->winding = winding;
}

BGD_DECLARE(void)
gdContextStroke(gdContextPtr context)
{
    gdContextStrokePreserve(context);
    gdContextNewPath(context);
}
