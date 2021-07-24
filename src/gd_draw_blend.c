
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "gd.h"
#include "gd_intern.h"
#include "gdhelpers.h"
#include "gd_errors.h"

#include "gd_surface.h"
#include "gd_array.h"
#include "gd_path_matrix.h"
#include "gd_span_rle.h"

#include "gd_fixed.h"

#define BILINEAR_INTERPOLATION_BITS 7
#define BILINEAR_INTERPOLATION_RANGE (1 << BILINEAR_INTERPOLATION_BITS)

#define CLIP(v, low, high) ((v) < (low) ? (low) : ((v) > (high) ? (high) : (v)))
#define MOD(a, b) ((a) < 0 ? ((b) - ((-(a) - 1) % (b))) - 1 : (a) % (b))

static inline int _fixed_to_bilinear_weight (gd_fixed_t x)
{
    return (x >> (16 - BILINEAR_INTERPOLATION_BITS)) &
           ((1 << BILINEAR_INTERPOLATION_BITS) - 1);
}

static inline uint32_t fetch_pixel_general (gdSurfacePtr surface, int x, int y, int check_bounds)
{
    if (check_bounds && (x < 0 || x >= surface->width || y < 0 || y >= surface->height))
    {
        return 0;
    }

    uint32_t *src = (uint32_t *)(surface->data);
    return src[y * surface->stride/4  + x];
}

#define SIZEOF_LONG 8
#if 1
#if SIZEOF_LONG > 4

static inline uint32_t bilinear_interpolation (uint32_t tl, uint32_t tr, uint32_t bl, uint32_t br, int distx, int disty)
{
    uint64_t distxy, distxiy, distixy, distixiy;
    uint64_t tl64, tr64, bl64, br64;
    uint64_t f, r;

    distx <<= (8 - BILINEAR_INTERPOLATION_BITS);
    disty <<= (8 - BILINEAR_INTERPOLATION_BITS);

    distxy = distx * disty;
    distxiy = distx * (256 - disty);
    distixy = (256 - distx) * disty;
    distixiy = (256 - distx) * (256 - disty);

    /* Alpha and Blue */
    tl64 = tl & 0xff0000ff;
    tr64 = tr & 0xff0000ff;
    bl64 = bl & 0xff0000ff;
    br64 = br & 0xff0000ff;

    f = tl64 * distixiy + tr64 * distxiy + bl64 * distixy + br64 * distxy;
    r = f & 0x0000ff0000ff0000ull;

    /* Red and Green */
    tl64 = tl;
    tl64 = ((tl64 << 16) & 0x000000ff00000000ull) | (tl64 & 0x0000ff00ull);

    tr64 = tr;
    tr64 = ((tr64 << 16) & 0x000000ff00000000ull) | (tr64 & 0x0000ff00ull);

    bl64 = bl;
    bl64 = ((bl64 << 16) & 0x000000ff00000000ull) | (bl64 & 0x0000ff00ull);

    br64 = br;
    br64 = ((br64 << 16) & 0x000000ff00000000ull) | (br64 & 0x0000ff00ull);

    f = tl64 * distixiy + tr64 * distxiy + bl64 * distixy + br64 * distxy;
    r |= ((f >> 16) & 0x000000ff00000000ull) | (f & 0xff000000ull);

    return (uint32_t)(r >> 16);
}

#else

static inline uint32_t
bilinear_interpolation (uint32_t tl, uint32_t tr,
        uint32_t bl, uint32_t br,
int distx, int disty)
{
    int distxy, distxiy, distixy, distixiy;
    uint32_t f, r;

    distx <<= (8 - BILINEAR_INTERPOLATION_BITS);
    disty <<= (8 - BILINEAR_INTERPOLATION_BITS);

    distxy = distx * disty;
    distxiy = (distx << 8) - distxy;	/* distx * (256 - disty) */
    distixy = (disty << 8) - distxy;	/* disty * (256 - distx) */
    distixiy =
    256 * 256 - (disty << 8) -
    (distx << 8) + distxy;		/* (256 - distx) * (256 - disty) */

    /* Blue */
    r = (tl & 0x000000ff) * distixiy + (tr & 0x000000ff) * distxiy
        + (bl & 0x000000ff) * distixy  + (br & 0x000000ff) * distxy;

    /* Green */
    f = (tl & 0x0000ff00) * distixiy + (tr & 0x0000ff00) * distxiy
        + (bl & 0x0000ff00) * distixy  + (br & 0x0000ff00) * distxy;
    r |= f & 0xff000000;

    tl >>= 16;
    tr >>= 16;
    bl >>= 16;
    br >>= 16;
    r >>= 16;

    /* Red */
    f = (tl & 0x000000ff) * distixiy + (tr & 0x000000ff) * distxiy
        + (bl & 0x000000ff) * distixy  + (br & 0x000000ff) * distxy;
    r |= f & 0x00ff0000;

    /* Alpha */
    f = (tl & 0x0000ff00) * distixiy + (tr & 0x0000ff00) * distxiy
        + (bl & 0x0000ff00) * distixy  + (br & 0x0000ff00) * distxy;
    r |= f & 0xff000000;

    return r;
}
#endif
#endif

static inline int _update_w_repeat (gdExtendMode repeat, int *c, int size)
{
    if (repeat == GD_EXTEND_NONE) {
        if (*c < 0 || *c >= size)
            return 0;
    }
    else if (repeat == GD_EXTEND_REPEAT)
    {
        while (*c >= size)
            *c -= size;
        while (*c < 0)
            *c += size;
    }
    else if (repeat == GD_EXTEND_PAD)
    {
        *c = CLIP (*c, 0, size - 1);
    }
    else /* REFLECT */
    {
    *c = MOD (*c, size * 2);
    if (*c >= size)
        *c = size * 2 - *c - 1;
    }
    return 1;
}

static inline uint32_t
bits_image_fetch_pixel_nearest (gdSurfacePtr image, gd_fixed_t x, gd_fixed_t y, gdExtendMode repeat_mode)
{
    int x0 = gd_fixed_to_int (x - gd_fixed_e);
    int y0 = gd_fixed_to_int (y - gd_fixed_e);
    if (repeat_mode != GD_EXTEND_NONE) {
        _update_w_repeat(repeat_mode, &x0, image->width);
        _update_w_repeat(repeat_mode, &y0, image->height);
        return fetch_pixel_general(image, x0, y0, 0);
    } else {
        return fetch_pixel_general(image, x0, y0, 1);
    }
}

static inline uint32_t
_surface_fetch_pixel_bilinear(gdSurfacePtr image, gd_fixed_t x, gd_fixed_t y, gdExtendMode repeat_mode)
{
    int width = image->width;
    int height = image->height;
    int x1, y1, x2, y2;
    uint32_t tl, tr, bl, br;
    int32_t distx, disty;

    x1 = x - gd_fixed_1 / 2;
    y1 = y - gd_fixed_1 / 2;

    distx = _fixed_to_bilinear_weight(x1);
    disty = _fixed_to_bilinear_weight(y1);
    x1 = gd_fixed_to_int(x1);
    y1 = gd_fixed_to_int(y1);
    x2 = x1 + 1;
    y2 = y1 + 1;
    if (repeat_mode != GD_EXTEND_NONE) {
        _update_w_repeat(repeat_mode, &x1, width);
        _update_w_repeat(repeat_mode, &y1, height);
        _update_w_repeat(repeat_mode, &x2, width);
        _update_w_repeat(repeat_mode, &y2, height);

        tl = fetch_pixel_general(image, x1, y1, 0);
        bl = fetch_pixel_general(image, x1, y2, 0);
        tr = fetch_pixel_general(image, x2, y1, 0);
        br = fetch_pixel_general(image, x2, y2, 0);
    } else {
        tl = fetch_pixel_general(image, x1, y1, 1);
        tr = fetch_pixel_general(image, x2, y1, 1);
        bl = fetch_pixel_general(image, x1, y2, 1);
        br = fetch_pixel_general(image, x2, y2, 1);
    }
    return bilinear_interpolation(tl, tr, bl, br, distx, disty);
}

static inline uint32_t BYTE_MUL(uint32_t x, uint32_t a)
{
    uint32_t t = (x & 0xff00ff) * a;
    t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
    t &= 0xff00ff;
    x = ((x >> 8) & 0xff00ff) * a;
    x = (x + ((x >> 8) & 0xff00ff) + 0x800080);
    x &= 0xff00ff00;
    x |= t;
    return x;
}

static inline void memfill32(uint32_t* dest, uint32_t value, int length)
{
    for(int i = 0 ;i < length;i++)
        *dest++ = value;
}

static inline uint32_t argb32_interpolate_pixel(uint32_t src, uint32_t src_alpha, uint32_t dst, uint32_t dst_alpha)
{
    uint32_t t = (src & 0xff00ff) * src_alpha + (dst & 0xff00ff) * dst_alpha;
    t >>= 8;
    t &= 0xff00ff;
    src = ((src >> 8) & 0xff00ff) * src_alpha + ((dst >> 8) & 0xff00ff) * dst_alpha;
    src &= 0xff00ff00;
    src |= t;
    return src;
}

#define ALPHA(c) ((c) >> 24)
static void operator_argb_color_source(uint32_t* dest, int length, uint32_t color, uint32_t alpha)
{
    if(alpha == 255)
    {
        memfill32(dest, color, length);
    }
    else
    {
        uint32_t ialpha = 255 - alpha;
        color = BYTE_MUL(color, alpha);
        for(int i = 0;i < length;i++)
            dest[i] = color + BYTE_MUL(dest[i], ialpha);
    }
}

static void operator_argb_color_source_over(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
{
    if(const_alpha != 255) color = BYTE_MUL(color, const_alpha);
    uint32_t ialpha = 255 - ALPHA(color);
    for(int i = 0;i < length;i++)
        dest[i] = color + BYTE_MUL(dest[i], ialpha);
}

static void operator_argb_color_destination_in(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
{
    uint32_t a = ALPHA(color);
    if(const_alpha != 255) a = BYTE_MUL(a, const_alpha) + 255 - const_alpha;
    for(int i = 0;i < length;i++)
        dest[i] = BYTE_MUL(dest[i], a);
}

static void operator_argb_color_destination_out(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
{
    uint32_t a = ALPHA(~color);
    if(const_alpha != 255) a = BYTE_MUL(a, const_alpha) + 255 - const_alpha;
    for(int i = 0; i < length;i++)
        dest[i] = BYTE_MUL(dest[i], a);
}

#define spanBlendLoop(func) \
    while(count--) \
    { \
        uint32_t* target = (uint32_t*)(surface->data + currentSpan->y * surface->stride) + currentSpan->x; \
        func(target, currentSpan->len, color, currentSpan->coverage); \
        ++currentSpan; \
    };

static void argb32_blend_color(gdSurfacePtr surface, gdImageOp op, const gdSpanRlePtr rle, uint32_t color)
{
    int count = rle->spans.size;
    gdSpanPtr currentSpan = rle->spans.data;
    switch (op) {
      case gdImageOpsSrc:
            spanBlendLoop(operator_argb_color_source);
            break;
      case gdImageOpsSrcOver:
            spanBlendLoop(operator_argb_color_source_over);
            break;
      case gdImageOpsDstIn:
            spanBlendLoop(operator_argb_color_destination_in);
            break;
      case gdImageOpsDstOut:
            spanBlendLoop(operator_argb_color_destination_out);
            break;
        default:
            gd_error("gdDraw does not implement yet GD_SURFACE_XRGB32.\n");
            break;
    }
}

static inline uint32_t premultiply_color(const gdColorPtr color, double opacity)
{
    const uint32_t alpha = (uint8_t)(color->a * opacity * 255);
    const uint32_t pr = (uint8_t)(color->r * alpha);
    const uint32_t pg = (uint8_t)(color->g * alpha);
    const uint32_t pb = (uint8_t)(color->b * alpha);

    return (alpha << 24) | (pr << 16) | (pg << 8) | (pb);
}

#define _getVarName(var)  #var
void gdBlendColor(gdContextPtr context, const gdSpanRlePtr rle, const gdColorPtr color)
{
    if(color==NULL || color->a==0.0)
        return;

    gdStatePtr state = context->state;
    // replace once we have more than 
    switch (context->surface->type) {
        case GD_SURFACE_ARGB32: {
                uint32_t pm_color = premultiply_color(color, state->opacity);
                argb32_blend_color(context->surface, state->op, rle, pm_color);
                break;
            }
      case GD_SURFACE_XRGB32:
      case GD_SURFACE_A8:
            gd_error("gdDraw does not implement %s yet.\n", _getVarName(GD_SURFACE_XRGB32));
            break;
      default:
            gd_error("gdDraw: provided surface has an unknown type.\n");
        return;
    }
}

typedef struct {
    gdPathMatrix matrix;
    gdExtendMode extend;
    uint8_t* data;
    gdSurfacePtr surface;
    int width;
    int height;
    int stride;
    int alpha;
} _spans_pattern;

// TODO: Once the rest is a tat bit faster, use func ptr for all but *_compose_source
// Macros are not an option, unreadable and painful to debug.
static void argb32_compose_source(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
{
    if(const_alpha == 255)
    {
        memcpy(dest, src, (size_t)(length) * sizeof(uint32_t));
    } else {
        uint32_t ialpha = 255 - const_alpha;
        for(int i = 0;i < length;i++)
            dest[i] = argb32_interpolate_pixel(src[i], const_alpha, dest[i], ialpha);
    }
}

static void argb32_compose_source_over(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
{
    uint32_t src_c, src_c_inv;
    if(const_alpha == 255) {
        for(int i = 0;i < length;i++) {
            src_c = src[i];
            if(src_c >= 0xff000000) dest[i] = src_c;
            else if(src_c != 0) {
                src_c_inv = ALPHA(~src_c);
                dest[i] = src_c + BYTE_MUL(dest[i], src_c_inv);
            }
        }
    } else {
        for(int i = 0;i < length;i++) {
            src_c = BYTE_MUL(src[i], const_alpha);
            src_c_inv = ALPHA(~src_c);
            dest[i] = src_c + BYTE_MUL(dest[i], src_c_inv);
        }
    }
}

static void argb32_compose_dst_out(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
{
    if(const_alpha == 255)
    {
        for(int i = 0;i < length;i++)
            dest[i] = BYTE_MUL(dest[i], ALPHA(~src[i]));
    }
    else
    {
        uint32_t cia = 255 - const_alpha;
        uint32_t sia;
        for(int i = 0;i < length;i++)
        {
            sia = BYTE_MUL(ALPHA(~src[i]), const_alpha) + cia;
            dest[i] = BYTE_MUL(dest[i], sia);
        }
    }
}

static void argb32_compose_dst_in(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
{
    if(const_alpha == 255)
    {
        for(int i = 0; i < length;i++)
            dest[i] = BYTE_MUL(dest[i], ALPHA(src[i]));
    }
    else
    {
        uint32_t cia = 255 - const_alpha;
        uint32_t a;
        for(int i = 0;i < length;i++)
        {
            a = BYTE_MUL(ALPHA(src[i]), const_alpha) + cia;
            dest[i] = BYTE_MUL(dest[i], a);
        }
    }
}

#define BUFFER_SIZE 1024
static void render_spans_compose_source(const gdSurface *surface, const _spans_pattern *pattern, uint32_t *buffer,
                                        const gdExtendMode extend, const int image_width_1616,
                                        const int image_height_1616, int fdx, int fdy, int count, gdSpanPtr spans) {
    while(count--) {
        uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + spans->x;
        const double cx = spans->x + 0.5;
        const double cy = spans->y + 0.5;
        int x = gd_double_to_fixed(pattern->matrix.m01 * cy + pattern->matrix.m00 * cx + pattern->matrix.m02);
        int y = gd_double_to_fixed(pattern->matrix.m11 * cy + pattern->matrix.m10 * cx + pattern->matrix.m12);
        const int coverage = (spans->coverage * pattern->alpha) >> 8;
        int length = spans->len;
        while(length) {
            int l = MIN(length, BUFFER_SIZE);
            const uint32_t* end = buffer + l;
            uint32_t* b = buffer;
            int px1616 = x;
            int py1616 = y;
            int px_delta_1616 = fdx;
            int py_delta_1616 = fdy;
            while(b < end) {
                int px = gd_fixed_to_int(px1616);
                int py = gd_fixed_to_int(py1616);
                *b = _surface_fetch_pixel_bilinear(pattern->surface, gd_int_to_fixed(px), gd_int_to_fixed(py), extend);
                x += fdx;
                y += fdy;
                px1616 += px_delta_1616;
                if(px1616 >= image_width_1616)
                    px1616 -= image_width_1616;
                py1616 += py_delta_1616;
                if(py1616 >= image_height_1616)
                    py1616 -= image_height_1616;
                ++b;
            }
            argb32_compose_source_over(target, l, buffer, coverage);
            target += l;
            length -= l;
        }
        ++spans;
    }
}

static void render_spans_compose_source_over(const gdSurface *surface, const _spans_pattern *pattern, uint32_t *buffer,
                                             const gdExtendMode extend, const int image_width_1616,
                                             const int image_height_1616, int fdx, int fdy, int count, gdSpanPtr spans) {
    while(count--) {
        uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + spans->x;
        const double cx = spans->x + 0.5;
        const double cy = spans->y + 0.5;
        int x = gd_double_to_fixed(pattern->matrix.m01 * cy + pattern->matrix.m00 * cx + pattern->matrix.m02);
        int y = gd_double_to_fixed(pattern->matrix.m11 * cy + pattern->matrix.m10 * cx + pattern->matrix.m12);
        const int coverage = (spans->coverage * pattern->alpha) >> 8;
        int length = spans->len;
        while(length) {
            int l = MIN(length, BUFFER_SIZE);
            const uint32_t* end = buffer + l;
            uint32_t* b = buffer;
            int px1616 = x;
            int py1616 = y;
            int px_delta_1616 = fdx;
            int py_delta_1616 = fdy;
            while(b < end) {
                int px = gd_fixed_to_int(px1616);
                int py = gd_fixed_to_int(py1616);
                *b = _surface_fetch_pixel_bilinear(pattern->surface, gd_int_to_fixed(px), gd_int_to_fixed(py), extend);
                x += fdx;
                y += fdy;
                px1616 += px_delta_1616;
                if(px1616 >= image_width_1616)
                    px1616 -= image_width_1616;
                py1616 += py_delta_1616;
                if(py1616 >= image_height_1616)
                    py1616 -= image_height_1616;
                ++b;
            }
            argb32_compose_source_over(target, l, buffer, coverage);
            target += l;
            length -= l;
        }
        ++spans;
    }
}

static void render_spans_compose_dst_in(const gdSurface *surface, const _spans_pattern *pattern, uint32_t *buffer,
                                        const gdExtendMode extend, const int image_width_1616,
                                        const int image_height_1616, int fdx, int fdy, int count, gdSpanPtr spans) {
    while(count--) {
        uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + spans->x;
        const double cx = spans->x + 0.5;
        const double cy = spans->y + 0.5;
        int x = gd_double_to_fixed(pattern->matrix.m01 * cy + pattern->matrix.m00 * cx + pattern->matrix.m02);
        int y = gd_double_to_fixed(pattern->matrix.m11 * cy + pattern->matrix.m10 * cx + pattern->matrix.m12);
        const int coverage = (spans->coverage * pattern->alpha) >> 8;
        int length = spans->len;
        while(length) {
            int l = MIN(length, BUFFER_SIZE);
            const uint32_t* end = buffer + l;
            uint32_t* b = buffer;
            int px1616 = x;
            int py1616 = y;
            int px_delta_1616 = fdx;
            int py_delta_1616 = fdy;
            while(b < end) {
                int px = gd_fixed_to_int(px1616);
                int py = gd_fixed_to_int(py1616);
                *b = _surface_fetch_pixel_bilinear(pattern->surface, gd_int_to_fixed(px), gd_int_to_fixed(py), extend);
                x += fdx;
                y += fdy;
                px1616 += px_delta_1616;
                if(px1616 >= image_width_1616)
                    px1616 -= image_width_1616;
                py1616 += py_delta_1616;
                if(py1616 >= image_height_1616)
                    py1616 -= image_height_1616;
                ++b;
            }
            argb32_compose_dst_in(target, l, buffer, coverage);
            target += l;
            length -= l;
        }
        ++spans;
    }
}

static void render_spans_compose_dst_out(const gdSurface *surface, const _spans_pattern *pattern, uint32_t *buffer,
                                         const gdExtendMode extend, const int image_width_1616,
                                         const int image_height_1616, int fdx, int fdy, int count, gdSpanPtr spans) {
    while(count--) {
        uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + spans->x;
        const double cx = spans->x + 0.5;
        const double cy = spans->y + 0.5;
        int x = gd_double_to_fixed(pattern->matrix.m01 * cy + pattern->matrix.m00 * cx + pattern->matrix.m02);
        int y = gd_double_to_fixed(pattern->matrix.m11 * cy + pattern->matrix.m10 * cx + pattern->matrix.m12);
        const int coverage = (spans->coverage * pattern->alpha) >> 8;
        int length = spans->len;
        while(length) {
            int l = MIN(length, BUFFER_SIZE);
            const uint32_t* end = buffer + l;
            uint32_t* b = buffer;
            int px1616 = x;
            int py1616 = y;
            int px_delta_1616 = fdx;
            int py_delta_1616 = fdy;
            while(b < end) {
                int px = gd_fixed_to_int(px1616);
                int py = gd_fixed_to_int(py1616);
                *b = _surface_fetch_pixel_bilinear(pattern->surface, gd_int_to_fixed(px), gd_int_to_fixed(py), extend);
                x += fdx;
                y += fdy;
                px1616 += px_delta_1616;
                if(px1616 >= image_width_1616)
                    px1616 -= image_width_1616;
                py1616 += py_delta_1616;
                if(py1616 >= image_height_1616)
                    py1616 -= image_height_1616;
                ++b;
            }
            argb32_compose_dst_out(target, l, buffer, coverage);
            target += l;
            length -= l;
        }
        ++spans;
    }
}
static void argb32_pattern_tiled_blend_transformed(gdSurfacePtr surface, gdImageOp op, const gdSpanRlePtr rle, const _spans_pattern * pattern)
{
    uint32_t buffer[BUFFER_SIZE];
    const gdExtendMode extend = pattern->extend;
    const int image_width_1616 = gd_int_to_fixed(pattern->width);
    const int image_height_1616 = gd_int_to_fixed(pattern->height);
    int fdx = gd_double_to_fixed(pattern->matrix.m00);
    int fdy = gd_double_to_fixed(pattern->matrix.m10);
    int count = rle->spans.size;
    gdSpanPtr spans = rle->spans.data;
    switch (op) {
        case gdImageOpsSrc:
            render_spans_compose_source(surface, pattern, buffer, extend, image_width_1616, image_height_1616, fdx,
                                        fdy, count, spans);
            break;
        case gdImageOpsSrcOver:
            render_spans_compose_source_over(surface, pattern, buffer, extend, image_width_1616, image_height_1616, fdx,
                                        fdy, count, spans);
            break;
        case gdImageOpsDstIn:
            render_spans_compose_dst_in(surface, pattern, buffer, extend, image_width_1616, image_height_1616, fdx,
                                             fdy, count, spans);
            break;
        case gdImageOpsDstOut:
            render_spans_compose_dst_out(surface, pattern, buffer, extend, image_width_1616, image_height_1616, fdx,
                                        fdy, count, spans);
            break;
        default:
            gd_error("Invalid Composition method %i.", op);
            break;
    }
}

#define spans_untransformed_blend_loop(composition) \
while(count--) { \
    int x = spans->x; \
    int length = spans->len; \
    int sx = xoff + x; \
    int sy = yoff + spans->y; \
    if(sy >= 0 && sy < image_height && sx < image_width) { \
        if(sx < 0) { \
            x -= sx; \
            length += sx; \
            sx = 0; \
        } \
        if(sx + length > image_width) length = image_width - sx; \
        if(length > 0) { \
            const int coverage = (spans->coverage * pattern->alpha) >> 8; \
            const uint32_t* src = (const uint32_t*)(pattern->data + sy * pattern->stride) + sx; \
            uint32_t* dest = (uint32_t*)(surface->data + spans->y * surface->stride) + x; \
            composition(dest, length, src, coverage); \
        } \
    } \
    ++spans; \
}

static void argb32_pattern_blend_untransformed(gdSurfacePtr surface, gdImageOp op, const gdSpanRlePtr rle, const _spans_pattern * pattern)
{
    const int image_width = pattern->width;
    const int image_height = pattern->height;
    int xoff = (int)(pattern->matrix.m02);
    int yoff = (int)(pattern->matrix.m12);

    int count = rle->spans.size;
    gdSpanPtr spans = rle->spans.data;

    switch (op) {
        case gdImageOpsSrc:
            spans_untransformed_blend_loop(argb32_compose_source);
            break;
        case gdImageOpsSrcOver:
            spans_untransformed_blend_loop(argb32_compose_source_over);
            break;
            spans_untransformed_blend_loop(argb32_compose_dst_in);
            break;
        case gdImageOpsDstOut:
            spans_untransformed_blend_loop(argb32_compose_dst_out);
            break;
        default:
            spans_untransformed_blend_loop(argb32_compose_source);
    }
}

void gdDrawBlendPattern(gdContextPtr context, const gdSpanRlePtr rle, const gdPathPatternPtr pattern)
{
    if(pattern == NULL || pattern->opacity == 0.0)
        return;

    gdStatePtr state = context->state;
    _spans_pattern pattern_impl;
    pattern_impl.extend = pattern->extend;
    pattern_impl.data = pattern->surface->data;
    pattern_impl.surface = pattern->surface;
    pattern_impl.width = pattern->surface->width;
    pattern_impl.height = pattern->surface->height;
    pattern_impl.stride = pattern->surface->stride;
    pattern_impl.alpha = (int)(state->opacity * pattern->opacity * 256.0);

    pattern_impl.matrix = pattern->matrix;
    gdPathMatrixMultiply(&pattern_impl.matrix, &pattern_impl.matrix, &state->matrix);
    gdPathMatrixInvert(&pattern_impl.matrix);

    const gdPathMatrixPtr matrix = &pattern_impl.matrix;
    int translating = (matrix->m00==1.0 && matrix->m10==0.0 && matrix->m01==0.0 && matrix->m11==1.0);
    if(translating) {
        if(pattern->extend == GD_EXTEND_NONE)
            argb32_pattern_blend_untransformed(context->surface, state->op, rle, &pattern_impl);
        else
            argb32_pattern_tiled_blend_transformed(context->surface, state->op, rle, &pattern_impl);
    } else {
        if(pattern->extend == GD_EXTEND_NONE)
            //argb32_pattern_blend_untransformed(context->surface, state->op, rle, &pattern_impl);
            argb32_pattern_tiled_blend_transformed(context->surface, state->op, rle, &pattern_impl);
        else
            argb32_pattern_tiled_blend_transformed(context->surface, state->op, rle, &pattern_impl);
    }
}

void gdPathBlend(gdContextPtr context, const gdSpanRlePtr rle)
{
    if(rle==NULL || rle->spans.size==0 || context->state->opacity==0.0)
        return;

    gdPaintPtr source = context->state->source;
    if(source->type==gdPaintTypeColor)
        gdBlendColor(context, rle, source->color);
    switch (source->type) {
        case gdPaintTypeColor:
            gdBlendColor(context, rle, source->color);
            break;
        case gdPaintTypePattern:
            gdDrawBlendPattern(context, rle, source->pattern);
            break;
        case gdPaintTypeGradient:
        case gdPaintTypeSurface:
        default:
            gd_error("Paint method not implemented or does not exist.");
            break;
    }
}
