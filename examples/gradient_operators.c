#include "gd.h"
#include <stdio.h>
#include <string.h>

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

static gdSurfacePtr render_tile(int op,int size)
{
    gdSurfacePtr s=gdSurfaceCreate(size,size,GD_SURFACE_ARGB32); gdContextPtr c=gdContextCreate(s); gdPaintPtr p;
    memset(s->data,0,(size_t)s->stride*s->height);
    p=gdPaintCreateLinear(0,0,size,size);
    gdPaintAddColorStopRgb(p,0,.05,.72,.86); gdPaintAddColorStopRgb(p,.5,.17,.16,.48); gdPaintAddColorStopRgb(p,1,.96,.32,.18);
    use(c,p); gdContextRectangle(c,12,30,size*.68,size*.68); gdContextFill(c);
    gdContextSetOperator(c,(gdCompositeOperator)op);
    p=gdPaintCreateRadial(size*.66,size*.40,2,size*.57,size*.52,size*.48);
    gdPaintAddColorStopRgba(p,0,1,.94,.20,.96); gdPaintAddColorStopRgba(p,.55,.96,.12,.63,.82);
    gdPaintAddColorStopRgba(p,1,.20,.02,.55,.12);
    use(c,p); gdContextArc(c,size*.58,size*.52,size*.43,0,2*M_PI); gdContextFill(c);
    gdContextSetOperator(c,GD_OP_OVER);
    gdContextSetSourceRgba(c,.01,.02,.06,.66); gdContextRectangle(c,0,0,size,23); gdContextFill(c);
    gdContextSetSourceRgba(c,1,1,1,.95); label(c,short_names[op],6,5,1.8);
    gdContextDestroy(c); return s;
}

static unsigned int over_checker(unsigned int src,int x,int y)
{
    unsigned int a=src>>24;
    unsigned int bg=((x/10+y/10)&1)?0xff8d8d8du:0xffc8c8c8u;
    unsigned int inv=255-a;
    unsigned int r=((src>>16)&255)+(((bg>>16)&255)*inv+127)/255;
    unsigned int g=((src>>8)&255)+(((bg>>8)&255)*inv+127)/255;
    unsigned int b=(src&255)+((bg&255)*inv+127)/255;
    return 0xff000000u|(r<<16)|(g<<8)|b;
}

int main(void)
{
    const int cols=5, tile=150, gap=8, rows=(GD_OP_COUNT+cols-1)/cols;
    const int w=cols*tile+(cols+1)*gap, h=rows*tile+(rows+1)*gap;
    gdSurfacePtr out=gdSurfaceCreate(w,h,GD_SURFACE_ARGB32); FILE *fp;
    if(!out)return 1;
    for(int y=0;y<h;y++) {
        unsigned int *row=(unsigned int *)(out->data+y*out->stride);
        for(int x=0;x<w;x++) row[x]=0xff12121au;
    }
    for(int op=0;op<GD_OP_COUNT;op++){
        gdSurfacePtr t=render_tile(op,tile); int ox=gap+(op%cols)*(tile+gap),oy=gap+(op/cols)*(tile+gap);
        for(int y=0;y<tile;y++) {
            unsigned int *src=(unsigned int *)(t->data+y*t->stride);
            unsigned int *dst=(unsigned int *)(out->data+(oy+y)*out->stride)+ox;
            for(int x=0;x<tile;x++)dst[x]=over_checker(src[x],x,y);
        }
        gdSurfaceDestroy(t); printf("%02d  %s\n",op,names[op]);
    }
    fp=fopen("gradient_operators.png","wb"); if(!fp){gdSurfaceDestroy(out);return 1;}
    gdSurfacePng(out,fp);fclose(fp);gdSurfaceDestroy(out);
    puts("Saved gradient_operators.png");return 0;
}
