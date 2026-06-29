#include "vector2d_example.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void use(gdContextPtr ctx, gdPaintPtr paint)
{
    gdContextSetSource(ctx, paint);
    gdPaintDestroy(paint);
}

int main(void)
{
    const int w=960, h=640;
    gdImagePtr image=vector2d_create_image(w,h,gdTrueColorAlpha(0,0,0,0));
    gdContextPtr c;
    gdPaintPtr p;
    unsigned int seed=0x51a7u;
    if(!image) return 1;
    c=gdContextCreateForImage(image); if(!c){gdImageDestroy(image);return 1;}

    /* A deep twilight sky. */
    p=gdPaintCreateLinear(0,0,0,430);
    gdPaintAddColorStopRgb(p,0,.025,.035,.14);
    gdPaintAddColorStopRgb(p,.48,.18,.08,.30);
    gdPaintAddColorStopRgb(p,.78,.83,.20,.30);
    gdPaintAddColorStopRgb(p,1,.98,.50,.25);
    use(c,p); gdContextRectangle(c,0,0,w,430); gdContextFill(c);

    /* A soft sun, with a transparent halo. */
    p=gdPaintCreateRadial(690,245,8,690,245,145);
    gdPaintAddColorStopRgba(p,0,1,.94,.62,1);
    gdPaintAddColorStopRgba(p,.18,1,.65,.27,.95);
    gdPaintAddColorStopRgba(p,1,1,.25,.12,0);
    use(c,p); gdContextArc(c,690,245,145,0,2*M_PI); gdContextFill(c);

    /* Stars are deterministic, so the example always produces the same art. */
    for(int i=0;i<85;i++) {
        seed=seed*1664525u+1013904223u;
        double x=18+(seed%924); seed=seed*1664525u+1013904223u;
        double y=15+(seed%210), r=(i%11==0)?1.8:.8;
        gdContextSetSourceRgba(c,1,.88,.74,.35+(i%5)*.12);
        gdContextArc(c,x,y,r,0,2*M_PI); gdContextFill(c);
    }

    /* Back and front mountain silhouettes. */
    p=gdPaintCreateLinear(0,255,0,475);
    gdPaintAddColorStopRgb(p,0,.25,.11,.29); gdPaintAddColorStopRgb(p,1,.055,.08,.18);
    use(c,p); gdContextMoveTo(c,0,430); gdContextLineTo(c,0,365);
    gdContextLineTo(c,130,275); gdContextLineTo(c,235,365); gdContextLineTo(c,365,235);
    gdContextLineTo(c,530,380); gdContextLineTo(c,650,292); gdContextLineTo(c,800,392);
    gdContextLineTo(c,960,300); gdContextLineTo(c,960,430); gdContextClosePath(c); gdContextFill(c);

    p=gdPaintCreateLinear(0,335,0,505);
    gdPaintAddColorStopRgb(p,0,.07,.15,.22); gdPaintAddColorStopRgb(p,1,.015,.045,.09);
    use(c,p); gdContextMoveTo(c,0,500); gdContextLineTo(c,0,420);
    gdContextCurveTo(c,120,350,190,370,285,455);
    gdContextCurveTo(c,390,350,500,350,610,455);
    gdContextCurveTo(c,730,365,830,350,960,425);
    gdContextLineTo(c,960,505); gdContextClosePath(c); gdContextFill(c);

    /* Water and the sunset's reflected path. */
    p=gdPaintCreateLinear(0,425,0,h);
    gdPaintAddColorStopRgb(p,0,.10,.16,.25); gdPaintAddColorStopRgb(p,.55,.025,.10,.17);
    gdPaintAddColorStopRgb(p,1,.008,.035,.08);
    use(c,p); gdContextRectangle(c,0,425,w,h-425); gdContextFill(c);

    p=gdPaintCreateLinear(520,0,835,0);
    gdPaintAddColorStopRgba(p,0,1,.32,.13,0); gdPaintAddColorStopRgba(p,.5,1,.66,.30,.72);
    gdPaintAddColorStopRgba(p,1,1,.30,.12,0);
    use(c,p); gdContextMoveTo(c,590,425); gdContextLineTo(c,790,425);
    gdContextLineTo(c,870,640); gdContextLineTo(c,475,640); gdContextClosePath(c); gdContextFill(c);

    /* Fine water glints. */
    gdContextSetLineWidth(c,1.2);
    for(int i=0;i<34;i++) {
        double y=438+i*5.7, half=25+i*4.2;
        gdContextSetSourceRgba(c,1,.70,.42,.32*(1-i/40.0));
        gdContextMoveTo(c,680-half,y); gdContextLineTo(c,680+half,y); gdContextStroke(c);
    }

    /* A tiny sailboat gives the gradients a story. */
    gdContextSetSourceRgba(c,.012,.018,.04,1);
    gdContextMoveTo(c,278,475); gdContextLineTo(c,410,475); gdContextLineTo(c,385,491);
    gdContextLineTo(c,302,491); gdContextClosePath(c); gdContextFill(c);
    gdContextSetLineWidth(c,4); gdContextMoveTo(c,345,475); gdContextLineTo(c,345,350); gdContextStroke(c);
    p=gdPaintCreateLinear(345,355,410,470);
    gdPaintAddColorStopRgb(p,0,1,.78,.50); gdPaintAddColorStopRgb(p,1,.72,.16,.22);
    use(c,p); gdContextMoveTo(c,350,360); gdContextLineTo(c,350,466);
    gdContextLineTo(c,423,466); gdContextClosePath(c); gdContextFill(c);

    gdContextFlushImage(c);
    if(!vector2d_save_png(image,"gradient_art.png")){gdContextDestroy(c);gdImageDestroy(image);return 1;}
    gdContextDestroy(c); gdImageDestroy(image);
    puts("Saved gradient_art.png"); return 0;
}
