#include "gd.h"
#include <stdio.h>
#include <string.h>

static const char *names[GD_OP_COUNT]={
 "CLEAR","SOURCE","OVER","IN","OUT","ATOP","DEST","DOVER","DIN","DOUT",
 "DATOP","XOR","ADD","SATURATE","MULTIPLY","SCREEN","OVERLAY","DARKEN",
 "LIGHTEN","DODGE","BURN","HARDLIGHT","SOFTLIGHT","DIFFERENCE","EXCLUSION",
 "HUE","SATURATION","COLOR","LUMINOSITY"
};

/* Tiny 5x7 capitals keep this example independent of font libraries. */
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
        const unsigned char *glyph;
        if(*text<'A'||*text>'Z')continue;
        glyph=font[*text-'A'];
        for(int row=0;row<7;row++)for(int col=0;col<5;col++)if(glyph[row]&(16>>col)) {
            gdContextRectangle(c,x+col*scale,y+row*scale,scale,scale);
            gdContextFill(c);
        }
    }
}

static gdSurfacePtr render_tile(gdCompositeOperator op,int size)
{
    gdSurfacePtr surface=gdSurfaceCreate(size,size,GD_SURFACE_ARGB32);
    gdContextPtr c=gdContextCreate(surface);

    /* Composite on genuine transparency: destination (red), then source (blue). */
    memset(surface->data,0,(size_t)surface->stride*surface->height);
    gdContextSetSourceRgb(c,.92,.03,.06);
    gdContextRectangle(c,18,30,92,82); gdContextFill(c);

    gdContextSetOperator(c,op);
    gdContextSetSourceRgb(c,.04,.12,1.0);
    gdContextRectangle(c,52,62,92,82); gdContextFill(c);

    /* Labels are annotations, not part of the operator comparison. */
    gdContextSetOperator(c,GD_OP_OVER);
    gdContextSetSourceRgba(c,.01,.01,.025,.76);
    gdContextRectangle(c,0,0,size,23); gdContextFill(c);
    gdContextSetSourceRgba(c,1,1,1,.96);
    label(c,names[op],6,5,1.8);

    gdContextDestroy(c);
    return surface;
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
    const int columns=5,tile=160,gap=8;
    const int rows=(GD_OP_COUNT+columns-1)/columns;
    const int width=columns*tile+(columns+1)*gap;
    const int height=rows*tile+(rows+1)*gap;
    gdSurfacePtr gallery=gdSurfaceCreate(width,height,GD_SURFACE_ARGB32);
    FILE *fp;
    if(!gallery)return 1;

    for(int y=0;y<height;y++) {
        unsigned int *row=(unsigned int *)(gallery->data+y*gallery->stride);
        for(int x=0;x<width;x++)row[x]=0xff181820u;
    }

    for(int op=0;op<GD_OP_COUNT;op++) {
        gdSurfacePtr tile_surface=render_tile((gdCompositeOperator)op,tile);
        int x=gap+(op%columns)*(tile+gap);
        int y=gap+(op/columns)*(tile+gap);
        for(int row=0;row<tile;row++) {
            unsigned int *src=(unsigned int *)(tile_surface->data+row*tile_surface->stride);
            unsigned int *dst=(unsigned int *)(gallery->data+(y+row)*gallery->stride)+x;
            for(int col=0;col<tile;col++)dst[col]=over_checker(src[col],col,row);
        }
        gdSurfaceDestroy(tile_surface);
        printf("%-12s destination=red, source=blue\n",names[op]);
    }

    fp=fopen("composite_operators.png","wb");
    if(!fp){gdSurfaceDestroy(gallery);return 1;}
    gdSurfacePng(gallery,fp);fclose(fp);gdSurfaceDestroy(gallery);
    puts("Saved composite_operators.png");
    return 0;
}
