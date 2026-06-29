#include "vector2d_example.h"
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const char *names[GD_OP_COUNT]={
 "CLEAR","SOURCE","OVER","IN","OUT","ATOP","DEST","DEST_OVER","DEST_IN",
 "DEST_OUT","DEST_ATOP","XOR","ADD","SATURATE","MULTIPLY","SCREEN","OVERLAY",
 "DARKEN","LIGHTEN","COLOR_DODGE","COLOR_BURN","HARD_LIGHT","SOFT_LIGHT",
 "DIFFERENCE","EXCLUSION","HSL_HUE","HSL_SATURATION","HSL_COLOR","HSL_LUMINOSITY"
};

static const char *short_names[GD_OP_COUNT]={
 "CLEAR","SOURCE","OVER","IN","OUT","ATOP","DEST","DOVER","DIN","DOUT",
 "DATOP","XOR","ADD","SATURATE","MULTIPLY","SCREEN","OVERLAY","DARKEN",
 "LIGHTEN","DODGE","BURN","HARDLIGHT","SOFTLIGHT","DIFFERENCE","EXCLUSION",
 "HUE","SATURATION","COLOR","LUMINOSITY"
};

static void use(gdContextPtr c, gdPaintPtr p){gdContextSetSource(c,p);gdPaintDestroy(p);}

static const unsigned char font[26][7]={
 {14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{14,17,16,16,16,17,14},
 {30,17,17,17,17,17,30},{31,16,16,30,16,16,31},{31,16,16,30,16,16,16},
 {14,17,16,23,17,17,15},{17,17,17,31,17,17,17},{14,4,4,4,4,4,14},
 {7,2,2,2,18,18,12},{17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
 {17,27,21,21,17,17,17},{17,25,21,19,17,17,17},{14,17,17,17,17,17,14},
 {30,17,17,30,16,16,16},{14,17,17,17,21,18,13},{30,17,17,30,20,18,17},
 {15,16,16,14,1,1,30},{31,4,4,4,4,4,4},{17,17,17,17,17,17,14},
 {17,17,17,17,17,10,4},{17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
 {17,17,10,4,4,4,4},{31,1,2,4,8,16,31}
};

static void label(gdContextPtr c,const char *text,double x,double y,double scale)
{
    for(;*text;text++,x+=6*scale) {
        if(*text<'A'||*text>'Z')continue;
        const unsigned char *g=font[*text-'A'];
        for(int row=0;row<7;row++)for(int col=0;col<5;col++)if(g[row]&(16>>col)) {
            gdContextRectangle(c,x+col*scale,y+row*scale,scale,scale);
            gdContextFill(c);
        }
    }
}

static gdImagePtr render_tile(int op,int size)
{
    gdImagePtr image=vector2d_create_image(size,size,gdTrueColorAlpha(255,255,255,127)); gdContextPtr c; gdPaintPtr p;
    if(!image)return NULL;
    c=gdContextCreateForImage(image); if(!c){gdImageDestroy(image);return NULL;}
    p=gdPaintCreateLinear(0,0,size,size);
    gdPaintAddColorStopRgb(p,0,.05,.72,.86); gdPaintAddColorStopRgb(p,.5,.17,.16,.48); gdPaintAddColorStopRgb(p,1,.96,.32,.18);
    use(c,p); gdContextRectangle(c,12,30,size*.68,size*.68); gdContextFill(c);
    gdContextSetOperator(c,(gdCompositeOperator)op);
    p=gdPaintCreateRadial(size*.66,size*.40,2,size*.57,size*.52,size*.48);
    gdPaintAddColorStopRgba(p,0,1,.94,.20,.96); gdPaintAddColorStopRgba(p,.55,.96,.12,.63,.82);
    gdPaintAddColorStopRgba(p,1,.20,.02,.55,.12);
    use(c,p); gdContextArc(c,size*.58,size*.52,size*.43,0,2*M_PI); gdContextFill(c);
    gdContextSetOperator(c,GD_OP_OVER);
    gdContextSetSourceRgba(c,.05,.05,.08,1); label(c,short_names[op],6,5,1.8);
    gdContextDestroy(c); return image;
}

int main(void)
{
    const int cols=5, tile=150, gap=8, rows=(GD_OP_COUNT+cols-1)/cols;
    const int w=cols*tile+(cols+1)*gap, h=rows*tile+(rows+1)*gap;
    gdImagePtr out=vector2d_create_image(w,h,gdTrueColorAlpha(255,255,255,127));
    if(!out)return 1;
    gdImageAlphaBlending(out,0);
    for(int op=0;op<GD_OP_COUNT;op++){
        gdImagePtr t=render_tile(op,tile); int ox=gap+(op%cols)*(tile+gap),oy=gap+(op/cols)*(tile+gap);
        if(!t){gdImageDestroy(out);return 1;}
        gdImageCopy(out,t,ox,oy,0,0,tile,tile);
        gdImageDestroy(t); printf("%02d  %s\n",op,names[op]);
    }
    gdImageAlphaBlending(out,1);
    if(!vector2d_save_png(out,"gradient_operators.png")){gdImageDestroy(out);return 1;}
    gdImageDestroy(out);
    puts("Saved gradient_operators.png");return 0;
}
