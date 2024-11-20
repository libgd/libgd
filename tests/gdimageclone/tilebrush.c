#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im, tile, brush, clone;

    im = gdImageCreate(50, 50);
    tile = gdImageCreateTrueColor(10, 10);
    brush = gdImageCreate(25, 25);
    gdImageColorTransparent(tile, 0xFFFFFF);
    gdImageSetTile(im, tile);
    gdImageSetTile(im, brush);

    clone = gdImageClone(im);

    gdImageDestroy(clone);
    gdImageDestroy(im);
    gdImageDestroy(brush);
    gdImageDestroy(tile);

    return 0;
}
