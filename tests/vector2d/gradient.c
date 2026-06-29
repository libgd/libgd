#include <math.h>
#include "gd_vector2d_private.h"
#include "gdtest.h"
#include "gd_gradient.h"
#include "gd_path_matrix.h"

static void close_pixel(gdPremulPixelF p, double r, double g, double b, double a)
{
    const double e = 0.003;
    gdTestAssert(fabs(p.r-r)<e && fabs(p.g-g)<e && fabs(p.b-b)<e && fabs(p.a-a)<e);
    gdTestAssert(isfinite(p.r)&&isfinite(p.g)&&isfinite(p.b)&&isfinite(p.a));
    gdTestAssert(p.r<=p.a+e&&p.g<=p.a+e&&p.b<=p.a+e);
}

static gdPremulPixelF sample(gdPaintPtr p, double x, double y)
{
    gdPathMatrix m;
    gdPathMatrixInitIdentity(&m);
    return gdGradientSample(p->gradient, &m, x, y);
}

static void test_stops_and_extend(void)
{
    gdPaintPtr p=gdPaintCreateLinear(0,0,10,0);
    gdTestAssert(p && p->gradient->extend==GD_EXTEND_PAD);
    close_pixel(sample(p,5,0),0,0,0,0);
    gdTestAssert(gdPaintAddColorStopRgba(p,0,1,0,0,0));
    close_pixel(sample(p,5,0),0,0,0,0);
    gdTestAssert(gdPaintAddColorStopRgba(p,1,0,0,1,1));
    close_pixel(sample(p,5,0),0,0,0.5,0.5);
    gdTestAssert(gdPaintSetExtend(p,GD_EXTEND_NONE));
    close_pixel(sample(p,-1,0),0,0,0,0);
    gdTestAssert(gdPaintSetExtend(p,GD_EXTEND_REPEAT));
    close_pixel(sample(p,15,0),0,0,0.5,0.5);
    gdTestAssert(gdPaintSetExtend(p,GD_EXTEND_REFLECT));
    close_pixel(sample(p,15,0),0,0,0.5,0.5);
    gdTestAssert(!gdPaintAddColorStopRgb(p,NAN,0,0,0));
    gdTestAssert(p->gradient->stop_count==2);
    gdPaintDestroy(p);
}

static void test_duplicates_and_matrix(void)
{
    gdPaintPtr p=gdPaintCreateLinear(0,0,1,0); gdPathMatrix m, singular;
    gdPaintAddColorStopRgb(p,0,1,0,0);
    gdPaintAddColorStopRgb(p,.5,1,0,0);
    gdPaintAddColorStopRgb(p,.5,0,1,0);
    gdPaintAddColorStopRgb(p,1,0,1,0);
    close_pixel(sample(p,.5,0),0,1,0,1);
    gdPathMatrixInitTranslate(&m,10,0);
    gdTestAssert(gdPaintSetMatrix(p,&m));
    gdPathMatrixInitScale(&singular,0,1);
    gdTestAssert(!gdPaintSetMatrix(p,&singular));
    gdTestAssert(p->gradient->matrix.m02==10);
    gdPaintDestroy(p);
}

static void test_radial(void)
{
    gdPaintPtr p=gdPaintCreateRadial(0,0,0,0,0,10);
    gdTestAssert(p && !gdPaintCreateRadial(0,0,-1,0,0,1));
    gdPaintAddColorStopRgb(p,0,1,0,0); gdPaintAddColorStopRgb(p,1,0,0,1);
    close_pixel(sample(p,5,0),.5,0,.5,1);
    gdPaintDestroy(p);
    p=gdPaintCreateRadial(0,0,2,0,0,2);
    gdPaintAddColorStopRgb(p,0,1,1,1);
    close_pixel(sample(p,2,0),0,0,0,0);
    gdPaintDestroy(p);
}

static void test_render(void)
{
    gdSurfacePtr s=gdSurfaceCreate(10,1,GD_SURFACE_ARGB32);
    gdContextPtr c=gdContextCreate(s); gdPaintPtr p=gdPaintCreateLinear(0,0,10,0);
    gdPaintAddColorStopRgb(p,0,1,0,0); gdPaintAddColorStopRgb(p,1,0,0,1);
    gdContextSetSource(c,p); gdContextRectangle(c,0,0,10,1); gdContextFill(c);
    gdTestAssert(((unsigned int *)s->data)[0]!=0 && ((unsigned int *)s->data)[9]!=0);
    gdPaintDestroy(p); gdContextDestroy(c); gdSurfaceDestroy(s);
}

int main(void)
{
    test_stops_and_extend(); test_duplicates_and_matrix(); test_radial(); test_render();
    return gdNumFailures();
}
