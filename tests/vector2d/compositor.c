#include <math.h>
#include <stdint.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd_vector2d_private.h"
#include "gdtest.h"
#include "gd_compositor.h"

typedef struct { double r, g, b, a; } RefPixel;
typedef struct { double r, g, b; } RefColor;

static double clamp1(double v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }

static RefPixel scale(RefPixel p, double c)
{
    p.r *= c; p.g *= c; p.b *= c; p.a *= c;
    return p;
}

static RefPixel clamp_pixel(RefPixel p)
{
    p.a = clamp1(p.a);
    p.r = fmin(clamp1(p.r), p.a);
    p.g = fmin(clamp1(p.g), p.a);
    p.b = fmin(clamp1(p.b), p.a);
    return p;
}

static RefPixel pd(gdCompositeOperator op, RefPixel s, RefPixel d)
{
    double fs = 0, fd = 0;
    RefPixel r;
    switch (op) {
    case GD_OP_CLEAR: break;
    case GD_OP_SOURCE: fs = 1; break;
    case GD_OP_OVER: fs = 1; fd = 1-s.a; break;
    case GD_OP_IN: fs = d.a; break;
    case GD_OP_OUT: fs = 1-d.a; break;
    case GD_OP_ATOP: fs = d.a; fd = 1-s.a; break;
    case GD_OP_DEST: fd = 1; break;
    case GD_OP_DEST_OVER: fs = 1-d.a; fd = 1; break;
    case GD_OP_DEST_IN: fd = s.a; break;
    case GD_OP_DEST_OUT: fd = 1-s.a; break;
    case GD_OP_DEST_ATOP: fs = 1-d.a; fd = s.a; break;
    case GD_OP_XOR: fs = 1-d.a; fd = 1-s.a; break;
    case GD_OP_ADD: fs = fd = 1; break;
    case GD_OP_SATURATE: fs = s.a > 0 ? fmin(1, (1-d.a)/s.a) : 0; fd = 1; break;
    default: break;
    }
    r.r=s.r*fs+d.r*fd; r.g=s.g*fs+d.g*fd; r.b=s.b*fs+d.b*fd; r.a=s.a*fs+d.a*fd;
    return clamp_pixel(r);
}

static double blend_channel(gdCompositeOperator op, double s, double d)
{
    switch (op) {
    case GD_OP_MULTIPLY: return s*d;
    case GD_OP_SCREEN: return s+d-s*d;
    case GD_OP_OVERLAY: return d<=.5 ? 2*s*d : 1-2*(1-s)*(1-d);
    case GD_OP_DARKEN: return fmin(s,d);
    case GD_OP_LIGHTEN: return fmax(s,d);
    case GD_OP_COLOR_DODGE: return s>=1 ? 1 : fmin(1,d/(1-s));
    case GD_OP_COLOR_BURN: return s<=0 ? 0 : 1-fmin(1,(1-d)/s);
    case GD_OP_HARD_LIGHT: return s<=.5 ? 2*s*d : 1-2*(1-s)*(1-d);
    case GD_OP_SOFT_LIGHT: {
        double g = d<=.25 ? ((16*d-12)*d+4)*d : sqrt(d);
        return s<=.5 ? d-(1-2*s)*d*(1-d) : d+(2*s-1)*(g-d);
    }
    case GD_OP_DIFFERENCE: return fabs(d-s);
    case GD_OP_EXCLUSION: return s+d-2*s*d;
    default: return s;
    }
}

static double lum(RefColor c) { return .3*c.r+.59*c.g+.11*c.b; }
static double sat(RefColor c) { return fmax(c.r,fmax(c.g,c.b))-fmin(c.r,fmin(c.g,c.b)); }

static RefColor clip_color(RefColor c)
{
    double l=lum(c), n=fmin(c.r,fmin(c.g,c.b)), x=fmax(c.r,fmax(c.g,c.b));
    if (n<0) { double k=l/(l-n); c.r=l+(c.r-l)*k; c.g=l+(c.g-l)*k; c.b=l+(c.b-l)*k; }
    x=fmax(c.r,fmax(c.g,c.b));
    if (x>1) { double k=(1-l)/(x-l); c.r=l+(c.r-l)*k; c.g=l+(c.g-l)*k; c.b=l+(c.b-l)*k; }
    return c;
}

static RefColor set_lum(RefColor c, double l)
{
    double d=l-lum(c); c.r+=d; c.g+=d; c.b+=d; return clip_color(c);
}

static RefColor set_sat(RefColor c, double s)
{
    double *v[3]={&c.r,&c.g,&c.b}, *t;
    if (*v[0]>*v[1]) {t=v[0];v[0]=v[1];v[1]=t;}
    if (*v[1]>*v[2]) {t=v[1];v[1]=v[2];v[2]=t;}
    if (*v[0]>*v[1]) {t=v[0];v[0]=v[1];v[1]=t;}
    if (*v[2]>*v[0]) {*v[1]=(*v[1]-*v[0])*s/(*v[2]-*v[0]);*v[2]=s;} else {*v[1]=*v[2]=0;}
    *v[0]=0; return c;
}

static RefColor hsl(gdCompositeOperator op, RefColor s, RefColor d)
{
    switch(op) {
    case GD_OP_HSL_HUE: return set_lum(set_sat(s,sat(d)),lum(d));
    case GD_OP_HSL_SATURATION: return set_lum(set_sat(d,sat(s)),lum(d));
    case GD_OP_HSL_COLOR: return set_lum(s,lum(d));
    default: return set_lum(d,lum(s));
    }
}

static RefPixel blend(gdCompositeOperator op, RefPixel s, RefPixel d)
{
    RefColor cs={0,0,0},cd={0,0,0},b; RefPixel r;
    if(s.a){cs.r=s.r/s.a;cs.g=s.g/s.a;cs.b=s.b/s.a;}
    if(d.a){cd.r=d.r/d.a;cd.g=d.g/d.a;cd.b=d.b/d.a;}
    if(op>=GD_OP_HSL_HUE)b=hsl(op,cs,cd);
    else {b.r=blend_channel(op,cs.r,cd.r);b.g=blend_channel(op,cs.g,cd.g);b.b=blend_channel(op,cs.b,cd.b);}
    r.a=s.a+d.a*(1-s.a);
    r.r=(1-d.a)*s.r+(1-s.a)*d.r+s.a*d.a*b.r;
    r.g=(1-d.a)*s.g+(1-s.a)*d.g+s.a*d.a*b.g;
    r.b=(1-d.a)*s.b+(1-s.a)*d.b+s.a*d.a*b.b;
    return clamp_pixel(r);
}

static RefPixel reference(gdCompositeOperator op, RefPixel s, RefPixel d, double coverage)
{
    RefPixel r;
    coverage=clamp1(coverage);
    if(op==GD_OP_CLEAR||op==GD_OP_SOURCE){
        r=pd(op,s,d);
        r.r=d.r+(r.r-d.r)*coverage;r.g=d.g+(r.g-d.g)*coverage;
        r.b=d.b+(r.b-d.b)*coverage;r.a=d.a+(r.a-d.a)*coverage;
        return clamp_pixel(r);
    }
    s=scale(s,coverage);
    return op<=GD_OP_SATURATE?pd(op,s,d):blend(op,s,d);
}

static void assert_close(gdPremulPixelF got, RefPixel exp)
{
    const double e=2e-5;
    gdTestAssert(fabs(got.r-exp.r)<e); gdTestAssert(fabs(got.g-exp.g)<e);
    gdTestAssert(fabs(got.b-exp.b)<e); gdTestAssert(fabs(got.a-exp.a)<e);
    gdTestAssert(isfinite(got.r)&&isfinite(got.g)&&isfinite(got.b)&&isfinite(got.a));
    gdTestAssert(got.r>=0&&got.g>=0&&got.b>=0&&got.a>=0&&got.a<=1);
    gdTestAssert(got.r<=got.a+e&&got.g<=got.a+e&&got.b<=got.a+e);
}

static uint32_t random_state=0x12345678u;
static double random_unit(void)
{
    random_state=random_state*1664525u+1013904223u;
    return (random_state>>8)/(double)0x1000000u;
}

static RefPixel random_pixel(void)
{
    RefPixel p; p.a=random_unit(); p.r=random_unit()*p.a; p.g=random_unit()*p.a; p.b=random_unit()*p.a; return p;
}

static gdPremulPixelF fp(RefPixel p)
{
    gdPremulPixelF r={(float)p.r,(float)p.g,(float)p.b,(float)p.a}; return r;
}

static void test_all_operators(void)
{
    static const double coverage[]={0,.125,.5,1};
    for(int op=0;op<GD_OP_COUNT;op++){
        gdTestAssert(gdCompositeOperatorIsValid((gdCompositeOperator)op));
        for(int i=0;i<128;i++){
            RefPixel s=random_pixel(),d=random_pixel();
            double c=coverage[i%4];
            assert_close(gdCompositePixel((gdCompositeOperator)op,fp(s),fp(d),(float)c),reference((gdCompositeOperator)op,s,d,c));
        }
    }
    gdTestAssert(!gdCompositeOperatorIsValid((gdCompositeOperator)-1));
    gdTestAssert(!gdCompositeOperatorIsValid(GD_OP_COUNT));
}

static void test_conversions_and_legacy(void)
{
    int src=gdTrueColorAlpha(220,30,90,43),dst=gdTrueColorAlpha(10,180,240,71);
    gdPremulPixelF s=gdCompositePixelFromGd(src),d=gdCompositePixelFromGd(dst);
#if ENABLE_CORRECTED_LEGACY_COMPOSITING
    gdTestAssert(gdAlphaBlend(dst,src)==gdCompositePixelToGd(gdCompositePixel(GD_OP_OVER,s,d,1)));
    gdTestAssert(gdLayerOverlay(dst,src)==gdCompositePixelToGd(gdCompositePixel(GD_OP_OVERLAY,s,d,1)));
    gdTestAssert(gdLayerMultiply(dst,src)==gdCompositePixelToGd(gdCompositePixel(GD_OP_MULTIPLY,s,d,1)));
#else
    gdTestAssert(gdAlphaBlend(dst,src)!=gdCompositePixelToGd(gdCompositePixel(GD_OP_OVER,s,d,1)));
    gdTestAssert(gdLayerOverlay(dst,src)!=gdCompositePixelToGd(gdCompositePixel(GD_OP_OVERLAY,s,d,1)));
    gdTestAssert(gdLayerMultiply(dst,src)!=gdCompositePixelToGd(gdCompositePixel(GD_OP_MULTIPLY,s,d,1)));
#endif
}

static void test_context_setter(void)
{
    gdSurfacePtr surface=gdSurfaceCreate(2,2,GD_SURFACE_ARGB32);
    gdContextPtr context=gdContextCreate(surface);
    gdContextSetOperator(context,GD_OP_HSL_COLOR);
    gdTestAssert(context->state->op==GD_OP_HSL_COLOR);
    gdContextSetOperator(context,(gdCompositeOperator)999);
    gdTestAssert(context->state->op==GD_OP_HSL_COLOR);
    gdContextSetOperator(NULL,GD_OP_OVER);
    gdContextDestroy(context); gdSurfaceDestroy(surface);
}

static void fill_surface(gdSurfacePtr surface, uint32_t pixel)
{
    for (int y=0;y<surface->height;y++) {
        uint32_t *row=(uint32_t *)(surface->data+y*surface->stride);
        for(int x=0;x<surface->width;x++) row[x]=pixel;
    }
}

static void test_unbounded_clip(void)
{
    gdSurfacePtr surface=gdSurfaceCreate(10,10,GD_SURFACE_ARGB32);
    gdContextPtr context;
    uint32_t blue=0xff0000ffu;
    fill_surface(surface,blue);
    context=gdContextCreate(surface);
    gdContextRectangle(context,1,1,8,8);
    gdContextClip(context);
    gdContextSetOperator(context,GD_OP_DEST_IN);
    gdContextSetSourceRgb(context,1,0,0);
    gdContextRectangle(context,4,4,2,2);
    gdContextFill(context);
    {
        uint32_t *data=(uint32_t *)surface->data;
        int stride=surface->stride/4;
        gdTestAssert(data[0]==blue);             /* outside clip */
        gdTestAssert(data[5*stride+5]==blue);    /* inside source */
        gdTestAssert(data[2*stride+2]==0);       /* zero source inside clip */
        gdTestAssert(data[9*stride+9]==blue);    /* outside clip */
    }
    gdContextDestroy(context); gdSurfaceDestroy(surface);
}

static void test_solid_pattern_parity(void)
{
    gdSurfacePtr sample=gdSurfaceCreate(1,1,GD_SURFACE_ARGB32);
    *(uint32_t *)sample->data=0x997a1f3du;
    for(int op=0;op<GD_OP_COUNT;op++) {
        gdSurfacePtr solid_surface=gdSurfaceCreate(8,8,GD_SURFACE_ARGB32);
        gdSurfacePtr pattern_surface=gdSurfaceCreate(8,8,GD_SURFACE_ARGB32);
        gdContextPtr solid_context,pattern_context;
        gdPathPatternPtr pattern;
        gdPaintPtr paint;
        fill_surface(solid_surface,0xb3204060u);
        fill_surface(pattern_surface,0xb3204060u);
        solid_context=gdContextCreate(solid_surface);
        pattern_context=gdContextCreate(pattern_surface);
        gdContextSetOperator(solid_context,(gdCompositeOperator)op);
        gdContextSetOperator(pattern_context,(gdCompositeOperator)op);
        gdContextSetSourceRgba(solid_context,.8,.2,.4,.6);
        pattern=gdPathPatternCreate(sample);
        gdPathPatternSetExtend(pattern,GD_EXTEND_REPEAT);
        paint=gdPaintCreateFromPattern(pattern);
        gdContextSetSource(pattern_context,paint);
        gdContextRectangle(solid_context,2,2,4,4);
        gdContextRectangle(pattern_context,2,2,4,4);
        gdContextFill(solid_context); gdContextFill(pattern_context);
        if (memcmp(solid_surface->data,pattern_surface->data,
                   (size_t)solid_surface->stride*solid_surface->height)!=0)
            gdTestErrorMsg("solid/pattern mismatch for operator %d: %08x/%08x\n",op,
                           ((uint32_t *)solid_surface->data)[3*solid_surface->stride/4+3],
                           ((uint32_t *)pattern_surface->data)[3*pattern_surface->stride/4+3]);
        gdPaintDestroy(paint); gdPathPatternDestroy(pattern);
        gdContextDestroy(solid_context); gdContextDestroy(pattern_context);
        gdSurfaceDestroy(solid_surface); gdSurfaceDestroy(pattern_surface);
    }
    gdSurfaceDestroy(sample);
}

int main(void)
{
    test_all_operators(); test_conversions_and_legacy(); test_context_setter();
    test_unbounded_clip();
    test_solid_pattern_parity();
    return gdNumFailures();
}
