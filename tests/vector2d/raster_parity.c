#include "gd_vector2d_private.h"
#include "gd_path.h"
#include "gd_path_dash.h"
#include "gd_path_matrix.h"
#include "gd_path_outline.h"
#include "gd_span_rle.h"
#include "ftraster/gd_ft_raster.h"
#include "gdtest.h"

#include <stdlib.h>
#include <string.h>

typedef struct span_result {
    gdSpan *spans;
    int size;
    int capacity;
    int x, y, w, h;
} span_result;

static void collect_spans(int count, const GD_FT_Span *spans, void *user)
{
    span_result *result = user;
    if (result->size + count > result->capacity) {
        int capacity = result->capacity ? result->capacity : 64;
        while (capacity < result->size + count)
            capacity *= 2;
        result->spans = realloc(result->spans,
                                (size_t)capacity * sizeof(*result->spans));
        gdTestAssert(result->spans != NULL);
        result->capacity = capacity;
    }
    for (int i = 0; i < count; i++) {
        gdSpan *out = &result->spans[result->size++];
        out->x = spans[i].x;
        out->y = spans[i].y;
        out->len = spans[i].len;
        out->coverage = spans[i].coverage;
    }
}

static void collect_bbox(int x, int y, int w, int h, void *user)
{
    span_result *result = user;
    result->x = x;
    result->y = y;
    result->w = w;
    result->h = h;
}

static void init_params(GD_FT_Raster_Params *params, span_result *result,
                        const gdRectF *clip)
{
    memset(params, 0, sizeof(*params));
    params->flags = GD_FT_RASTER_FLAG_DIRECT | GD_FT_RASTER_FLAG_AA;
    params->gray_spans = collect_spans;
    params->bbox_cb = collect_bbox;
    params->user = result;
    if (clip) {
        params->flags |= GD_FT_RASTER_FLAG_CLIP;
        params->clip_box.xMin = (GD_FT_Pos)clip->x;
        params->clip_box.yMin = (GD_FT_Pos)clip->y;
        params->clip_box.xMax = (GD_FT_Pos)(clip->x + clip->w);
        params->clip_box.yMax = (GD_FT_Pos)(clip->y + clip->h);
    }
}

static int render_direct(gdPathPtr path, gdPathMatrixPtr matrix,
                         const gdRectF *clip, gdFillRule winding,
                         span_result *result)
{
    GD_FT_Raster_Params params;
    init_params(&params, result, clip);
    return gd_ft_raster_render_path(
        path, matrix, &params,
        winding == gdFillRulEvenOdd ? GD_FT_OUTLINE_EVEN_ODD_FILL
                                    : GD_FT_OUTLINE_NONE);
}

static int render_legacy(gdPathPtr path, gdPathMatrixPtr matrix,
                         const gdRectF *clip, gdFillRule winding,
                         span_result *result)
{
    GD_FT_Raster_Params params;
    init_params(&params, result, clip);
    GD_FT_Outline *outline = gd_ft_outline_convert(path, matrix);
    if (!outline)
        return -1;
    outline->flags = winding == gdFillRulEvenOdd
                         ? GD_FT_OUTLINE_EVEN_ODD_FILL
                         : GD_FT_OUTLINE_NONE;
    params.source = outline;
    int error = gd_ft_grays_raster.raster_render(NULL, &params);
    gd_ft_outline_destroy(outline);
    return error;
}

static void assert_parity(const char *name, gdPathPtr path,
                          gdPathMatrixPtr matrix, const gdRectF *clip,
                          gdFillRule winding)
{
    span_result direct = {0};
    span_result legacy = {0};
    int direct_error = render_direct(path, matrix, clip, winding, &direct);
    int legacy_error = render_legacy(path, matrix, clip, winding, &legacy);

    gdTestAssertMsg(direct_error == legacy_error,
                    "%s: errors differ (%d != %d)", name,
                    direct_error, legacy_error);
    gdTestAssertMsg(direct.size == legacy.size,
                    "%s: span counts differ (%d != %d)", name,
                    direct.size, legacy.size);
    gdTestAssertMsg(direct.x == legacy.x && direct.y == legacy.y &&
                        direct.w == legacy.w && direct.h == legacy.h,
                    "%s: bounding boxes differ", name);
    if (direct.size == legacy.size) {
        for (int i = 0; i < direct.size; i++) {
            gdSpan *a = &direct.spans[i];
            gdSpan *b = &legacy.spans[i];
            gdTestAssertMsg(a->x == b->x && a->y == b->y &&
                                a->len == b->len &&
                                a->coverage == b->coverage,
                            "%s: span %d differs", name, i);
        }
    }
    free(direct.spans);
    free(legacy.spans);
}

static gdPathPtr make_curve_path(void)
{
    gdPathPtr path = gdPathCreate();
    gdPathMoveTo(path, -12.5, 8.25);
    gdPathQuadTo(path, 25, -20, 48.5, 12);
    gdPathCurveTo(path, 70, 40, 20, 75, -5, 52);
    gdPathLineTo(path, -12.5, 8.25);
    gdPathClose(path);
    return path;
}

static gdPathPtr make_multi_contour(void)
{
    gdPathPtr path = gdPathCreate();
    gdPathAddRectangle(path, 5, 5, 70, 70);
    gdPathAddRectangle(path, 20, 20, 40, 40);
    gdPathMoveTo(path, 10, 40);
    gdPathLineTo(path, 70, 10);
    gdPathLineTo(path, 70, 70);
    gdPathClose(path);
    return path;
}

static void test_basic_corpus(void)
{
    gdPathMatrix matrix;
    gdPathMatrixInitIdentity(&matrix);
    gdRectF clip = {0, 0, 96, 96};

    gdPathPtr empty = gdPathCreate();
    assert_parity("empty", empty, &matrix, &clip, gdFillRuleNonZero);
    gdPathDestroy(empty);

    gdPathPtr move_only = gdPathCreate();
    gdPathMoveTo(move_only, 10, 10);
    assert_parity("move-only", move_only, &matrix, &clip,
                  gdFillRuleNonZero);
    gdPathDestroy(move_only);

    gdPathPtr tiger_reduced = gdPathCreate();
    gdPathLineTo(tiger_reduced, -67.25, 173.75);
    gdPathMoveTo(tiger_reduced, -39, 331);
    assert_parity("leading-line-tiger", tiger_reduced, &matrix, NULL,
                  gdFillRuleNonZero);
    gdPathDestroy(tiger_reduced);

    gdPathPtr leading_quad = gdPathCreate();
    gdPathQuadTo(leading_quad, 15, 2, 28, 20);
    gdPathMoveTo(leading_quad, 40, 40);
    assert_parity("leading-quad", leading_quad, &matrix, &clip,
                  gdFillRuleNonZero);
    gdPathDestroy(leading_quad);

    gdPathPtr leading_cubic = gdPathCreate();
    gdPathCurveTo(leading_cubic, 4, 5, 16, 30, 32, 18);
    gdPathMoveTo(leading_cubic, 45, 45);
    assert_parity("leading-cubic", leading_cubic, &matrix, &clip,
                  gdFillRuleNonZero);
    gdPathDestroy(leading_cubic);

    gdPathPtr curves = make_curve_path();
    assert_parity("curves", curves, &matrix, &clip, gdFillRuleNonZero);
    gdPathMatrixInitRotateTranslate(&matrix, 0.37, 18, 11);
    assert_parity("curves-transformed", curves, &matrix, &clip,
                  gdFillRuleNonZero);
    gdPathDestroy(curves);

    gdPathMatrixInitIdentity(&matrix);
    gdPathPtr multi = make_multi_contour();
    assert_parity("multi-nonzero", multi, &matrix, &clip,
                  gdFillRuleNonZero);
    assert_parity("multi-evenodd", multi, &matrix, &clip,
                  gdFillRulEvenOdd);
    gdPathDestroy(multi);
}

static void test_stroke_corpus(void)
{
    gdPathPtr center = gdPathCreate();
    gdPathMoveTo(center, 15, 15);
    gdPathLineTo(center, 55, 65);
    gdPathLineTo(center, 85, 20);
    gdPathMatrix matrix;
    gdPathMatrixInitIdentity(&matrix);
    gdRectF clip = {0, 0, 100, 100};
    gdStroke stroke = {12, 4, gdLineCapButt, gdLineJoinMiter, NULL};

    for (int cap = gdLineCapButt; cap <= gdLineCapSquare; cap++) {
        for (int join = gdLineJoinMiter; join <= gdLineJoinBevel; join++) {
            stroke.cap = (gdLineCap)cap;
            stroke.join = (gdLineJoin)join;
            gdPathPtr stroked = gdPathStrokeToPath(center, &stroke, &matrix);
            gdTestAssert(stroked != NULL);
            if (stroked) {
                assert_parity("stroke-cap-join", stroked, &matrix, &clip,
                              gdFillRuleNonZero);
                gdPathDestroy(stroked);
            }
        }
    }

    const double dash_values[] = {9, 4, 2, 4};
    gdPathDashPtr dash = gdPathDashCreate(dash_values, 4, 3);
    gdPathPtr dashed = gdPathApplyDash(dash, center);
    gdPathPtr stroked = gdPathStrokeToPath(dashed, &stroke, &matrix);
    gdTestAssert(stroked != NULL);
    if (stroked) {
        assert_parity("dashed-stroke", stroked, &matrix, &clip,
                      gdFillRuleNonZero);
        gdPathDestroy(stroked);
    }
    gdPathDestroy(dashed);
    gdPathDashDestroy(dash);
    gdPathDestroy(center);
}

static void test_band_subdivision(void)
{
    gdPathPtr path = gdPathCreate();
    for (int y = 0; y < 700; y += 7) {
        gdPathMoveTo(path, 0, y);
        for (int x = 0; x < 700; x += 7)
            gdPathLineTo(path, x, y + ((x / 7) & 1 ? 6 : 0));
        gdPathLineTo(path, 699, y + 6);
        gdPathLineTo(path, 0, y);
        gdPathClose(path);
    }
    gdPathMatrix matrix;
    gdPathMatrixInitIdentity(&matrix);
    gdRectF clip = {0, 0, 700, 700};
    assert_parity("band-subdivision", path, &matrix, &clip,
                  gdFillRuleNonZero);
    gdPathDestroy(path);
}

int main(void)
{
    test_basic_corpus();
    test_stroke_corpus();
    test_band_subdivision();
    return gdNumFailures();
}
