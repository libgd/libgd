/**
 * Test point ordering of gdImageRectangle()
 *
 * We're testing all four possible point orders with and without thickness,
 * and verify that all sides of the rectangle are drawn.
 *
 * See also <https://github.com/libgd/libgd/issues/177>
 */


#include "gd.h"
#include "gdtest.h"


static void draw_and_check_rectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int black, int red);
static void draw_and_check_pixel(gdImagePtr im, int x, int y, int black, int red);


int main()
{
    gdImagePtr im;
    int black, red;

    im = gdImageCreate(110, 210);
    gdImageColorAllocate(im, 255, 255, 255);
    black = gdImageColorAllocate(im,   0,   0,   0);
    red   = gdImageColorAllocate(im, 255,   0,   0);

    draw_and_check_rectangle(im,  10,  10,  50,  50, black, red);
    draw_and_check_rectangle(im,  50,  60,  10, 100, black, red);
    draw_and_check_rectangle(im,  50, 150,  10, 110, black, red);
    draw_and_check_rectangle(im,  10, 200,  50, 160, black, red);
    gdImageSetThickness(im, 4);
    draw_and_check_rectangle(im,  60,  10, 100,  50, black, red);
    draw_and_check_rectangle(im, 100,  60,  60, 100, black, red);
    draw_and_check_rectangle(im, 100, 150,  60, 110, black, red);
    draw_and_check_rectangle(im,  60, 200, 100, 160, black, red);

#if 0
    /* save image for debugging purposes */
    do {
        char *path;
        FILE *fp;

        path = gdTestFilePath2("gdimagerectangle", "point_ordering.png");
        fp = fopen(path, "wb");
        gdImagePng(im, fp);
        fclose(fp);
        gdFree(path);
    } while(0);
#endif

    gdImageDestroy(im);

    return gdNumFailures();
}


static void draw_and_check_rectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int black, int red)
{
    int x, y;

    gdImageRectangle(im, x1, y1, x2, y2, black);
    x = (x1 + x2) / 2;
    y = (y1 + y2) / 2;
    draw_and_check_pixel(im, x,  y1, black, red);
    draw_and_check_pixel(im, x1, y,  black, red);
    draw_and_check_pixel(im, x,  y2, black, red);
    draw_and_check_pixel(im, x2, y,  black, red);
}


static void draw_and_check_pixel(gdImagePtr im, int x, int y, int black, int red)
{
    int color;

    color = gdImagePalettePixel(im, x, y);
    gdTestAssertMsg(color == black, "expected color %d, but got color %d\n", black, color);
    gdImageSetPixel(im, x, y, red);
}
