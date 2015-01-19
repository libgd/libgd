#include "gd.h"

#include <stdio.h>
#include <assert.h> /* quick-and-dirty error checking. */
#include "gdtest.h"


#define MODE GD_BICUBIC    

int main() {
    /* Declare the image */
    gdImagePtr im, im2;
    FILE *in, *out;
    int stat;

    in = fopen(GDTEST_TOP_DIR "/gdinterpolatedscale/bug00091.jpg", "rb");
    assert(in);

    im = gdImageCreateFromJpeg(in);
    assert(im);

    fclose(in);

    stat = gdImageSetInterpolationMethod(im, MODE);
    assert(stat);

    im2 = gdImageScale(im, 260, 160);
    assert(im2);

    out = fopen("out.jpg", "wb");
    assert(out);

    gdImageJpeg(im2, out, 100);

    fclose(out);

    return 0;
}/* main*/
