#include "vector2d_example.h"
#include <stdio.h>

static const char *names[GD_OP_COUNT]={
 "CLEAR","SOURCE","OVER","IN","OUT","ATOP","DEST","DEST_OVER","DEST_IN",
 "DEST_OUT","DEST_ATOP","XOR","ADD","SATURATE","MULTIPLY","SCREEN","OVERLAY",
 "DARKEN","LIGHTEN","COLOR_DODGE","COLOR_BURN","HARD_LIGHT","SOFT_LIGHT",
 "DIFFERENCE","EXCLUSION","HSL_HUE","HSL_SATURATION","HSL_COLOR","HSL_LUMINOSITY"
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

static void render_example(gdContextPtr c,gdCompositeOperator op,double x,double y)
{
    /* Composite on genuine transparency: destination (red), then source (blue). */
    gdContextSetOperator(c,GD_OP_OVER);
    gdContextSetSourceRgba(c,.7,0,0,.8);
    gdContextRectangle(c,x,y,120,90); gdContextFill(c);

    gdContextSetOperator(c,op);
    gdContextSetSourceRgba(c,0,0,.9,.4);
    gdContextRectangle(c,x+40,y+30,120,90); gdContextFill(c);
}

int main(void)
{
    const int columns=5,tile_width=160,tile_height=120,label_height=18,gap=8;
    const int rows=(GD_OP_COUNT+columns-1)/columns;
    const int width=columns*tile_width+(columns+1)*gap;
    const int height=rows*(label_height+tile_height)+(rows+1)*gap;
    gdImagePtr gallery=vector2d_create_image(width,height,gdTrueColorAlpha(255,255,255,127));
    if(!gallery)return 1;

    for(int op=0;op<GD_OP_COUNT;op++) {
        int x=gap+(op%columns)*(tile_width+gap);
        int y=gap+(op/columns)*(label_height+tile_height+gap);
        gdContextPtr tile_context=gdContextCreateForImage(gallery);
        if(!tile_context){gdImageDestroy(gallery);return 1;}
        gdContextRectangle(tile_context,x,y+label_height,tile_width,tile_height);
        gdContextClip(tile_context);
        render_example(tile_context,(gdCompositeOperator)op,x,y+label_height);
        gdContextDestroy(tile_context);
        printf("%-12s destination=red, source=blue\n",names[op]);
    }

    {
        gdContextPtr labels=gdContextCreateForImage(gallery);
        if(!labels){gdImageDestroy(gallery);return 1;}
        gdContextSetSourceRgb(labels,.05,.05,.08);
        for(int op=0;op<GD_OP_COUNT;op++) {
            int x=gap+(op%columns)*(tile_width+gap);
            int y=gap+(op/columns)*(label_height+tile_height+gap);
            label(labels,names[op],x,y,1.5);
        }
        gdContextDestroy(labels);
    }
    if(!vector2d_save_png(gallery,"composite_operators.png")){gdImageDestroy(gallery);return 1;}
    gdImageDestroy(gallery);
    puts("Saved composite_operators.png");
    return 0;
}
