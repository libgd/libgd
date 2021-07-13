
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

static inline uint32_t interpolate_pixel(uint32_t x, uint32_t a, uint32_t y, uint32_t b)
{
    uint32_t t = (x & 0xff00ff) * a + (y & 0xff00ff) * b;
    t >>= 8;
    t &= 0xff00ff;
    x = ((x >> 8) & 0xff00ff) * a + ((y >> 8) & 0xff00ff) * b;
    x &= 0xff00ff00;
    x |= t;
    return x;
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
      case gdImageOpsSrcIn:
            spanBlendLoop(operator_argb_color_destination_in);
            break;
      case gdImageOpsSrcOut:
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
    //argb32_blend_color(context->surface, state->op, rle, pm_color);
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
        case gdPaintTypeGradient:
            //Not implemented
            break;
        case gdPaintTypeSurface:
            //Not implemented
            break;
        default:
            break;
    }
}
