#include "gd.h"
#include "gdtest.h"


#define SRC_WIDTH 80
#define SRC_HEIGHT 42
#define DEST_WIDTH 200
#define DEST_HEIGHT 200


/* Create an image and fill it with true transparency */
gdImagePtr blank_image(int width, int height)
{
    gdImagePtr img;
    int c;

    img = gdImageCreateTrueColor(width, height);
    gdImageAlphaBlending(img, 0);
    c = gdImageColorAllocateAlpha(img, 255, 0, 255, 127);
    gdImageFilledRectangle(img, 0, 0, width-1, height-1, c);
    return img;
}


/* Merge layers on top of each other */
gdImagePtr flatten(gdImagePtr *layers, int layer_count, int width, int height)
{
    gdImagePtr img;
    int i;

    img = blank_image(width,height);
    gdImageAlphaBlending(img, 1);
    for (i = 0; i < layer_count; i++) {
        gdImageCopy(img, layers[i], 0,0, 0,0, width,height);
    }
    gdImageSaveAlpha(img, 1);
    return img;
}


int main()
{
    gdImagePtr layers[2], background, logo_source, logo, scaled_logo, img;
    FILE *fp;

    background = blank_image(DEST_WIDTH,DEST_HEIGHT);

    fp = gdTestFileOpen2("gdimagecopyresampled", "bug00201_src.png");
    logo_source = gdImageCreateFromPng(fp);
    fclose(fp);

    logo = blank_image(SRC_WIDTH,SRC_HEIGHT);
    gdImageAlphaBlending(logo, 0);
    gdImageCopy(logo, logo_source, 0,0, 0,0, SRC_WIDTH,SRC_HEIGHT);
    gdImageDestroy(logo_source);

    scaled_logo = blank_image(DEST_WIDTH,DEST_HEIGHT);
    gdImageAlphaBlending(scaled_logo, 0);
    gdImageCopyResampled(scaled_logo, logo, 0,0, 0,0, 200,105, SRC_WIDTH,SRC_HEIGHT);
    gdImageDestroy(logo);

    layers[0] = background;
    layers[1] = scaled_logo;
    img = flatten(layers, 2, DEST_WIDTH, DEST_HEIGHT);
    gdImageDestroy(background);
    gdImageDestroy(scaled_logo);

    gdAssertImageEqualsToFile("gdimagecopyresampled/bug00201_exp.png", img);
    gdImageDestroy(img);
    return gdNumFailures();
}
