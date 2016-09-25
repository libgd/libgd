/**
 * Testing the basic functionality of gdImageGammaCorrect()
 */


#include "gd.h"
#include "gdtest.h"


static void test_gamma_both(double gamma);
static void test_gamma(double gamma, int truecolor);
static void draw_cell(gdImagePtr im, int x, int y);
static int cell_color(gdImagePtr im, int x, int y);


int main()
{
    test_gamma_both(0.5);
    test_gamma_both(1.0);
    test_gamma_both(2.0);

    return gdNumFailures();
}


static void test_gamma_both(double gamma)
{
    test_gamma(gamma, 0);
    test_gamma(gamma, 1);
}


static void test_gamma(double gamma, int truecolor)
{
    gdImagePtr im;
    int i, j;
    char basename[256];
    char *path;

    if (truecolor) {
        im = gdImageCreateTrueColor(640, 480);
    } else {
        im = gdImageCreate(640, 480);
    }
    for (j = 0; j < 4; j++) {
        for (i = 0; i < 32; i++) {
            draw_cell(im, i, j);
        }
    }

    gdImageGammaCorrect(im, gamma);

    sprintf(basename, "basic_%3.1f.png", gamma);
    path = gdTestFilePath2("gdimagegammacorrect", basename);

    gdAssertImageEqualsToFile(path, im);

    gdFree(path);
    gdImageDestroy(im);
}


static void draw_cell(gdImagePtr im, int x, int y)
{
    int x1, y1, x2, y2, color;

    x1 = 20 * x;
    y1 = 120 * y;
    x2 = x1 + 19;
    y2 = y1 + 119;
    color = cell_color(im, x, y);
    gdImageFilledRectangle(im, x1,y1, x2,y2, color);
}


static int cell_color(gdImagePtr im, int x, int y)
{
    int channel = 8 * x + 4;

    switch (y) {
        case 0:
            return gdImageColorAllocate(im, channel, channel, channel);
        case 1:
            return gdImageColorAllocate(im, channel, 0, 0);
        case 2:
            return gdImageColorAllocate(im, 0, channel, 0);
        case 3:
            return gdImageColorAllocate(im, 0, 0, channel);
    }
    return -1;
}
