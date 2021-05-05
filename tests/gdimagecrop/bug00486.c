/**
 * Test that gdImageCropAuto() crops left and right as well
 *
 * We test that an image with four unicolored quadrants, where either both left
 * quadrants or both right quadrants have the same color, but the remaining
 * quadrants have different colors, are treated identically by
 * gdImageCropAuto(…, GD_CROP_SIDES).
 *
 * See <https://github.com/libgd/libgd/issues/486>
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr orig, croppedLeft, croppedRight;
    int red, green, blue;

    orig = gdImageCreateTrueColor(8, 8);
    red = gdImageColorAllocate(orig, 255, 0, 0);
    green = gdImageColorAllocate(orig, 0, 255, 0);
    blue = gdImageColorAllocate(orig, 0, 0, 255);

    gdImageFilledRectangle(orig, 0, 0, 3, 3, green); // tl
    gdImageFilledRectangle(orig, 4, 0, 7, 3, red);   // tr
    gdImageFilledRectangle(orig, 0, 4, 3, 7, green); // bl
    gdImageFilledRectangle(orig, 4, 4, 7, 7, blue);  // br
    croppedLeft = gdImageCropAuto(orig, GD_CROP_SIDES);
    gdTestAssert(croppedLeft != NULL);

    gdImageFilledRectangle(orig, 0, 0, 3, 3, red);   // tl
    gdImageFilledRectangle(orig, 4, 0, 7, 3, green); // tr
    gdImageFilledRectangle(orig, 0, 4, 3, 7, blue);  // bl
    gdImageFilledRectangle(orig, 4, 4, 7, 7, green); // br
    croppedRight = gdImageCropAuto(orig, GD_CROP_SIDES);
    gdTestAssert(croppedRight != NULL);

    gdAssertImageEquals(croppedRight, croppedLeft);

	gdImageDestroy(orig);
	gdImageDestroy(croppedLeft);
	gdImageDestroy(croppedRight);

    return gdNumFailures();
}
