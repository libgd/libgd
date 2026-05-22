#include "gd.h"
#include "gdtest.h"

int main()
{
  gdImagePtr im;
  im = gdImageCreate(1, 1);
  gdTestAssert(gdImageSetInterpolationMethod(im, GD_BILINEAR_FIXED) != 0);
  gdTestAssert(gdImageGetInterpolationMethod(im) == GD_BILINEAR_FIXED);
  gdTestAssert(gdImageSetInterpolationMethod(im, GD_BICUBIC_FIXED) != 0);
  gdTestAssert(gdImageGetInterpolationMethod(im) == GD_BICUBIC_FIXED);
  gdImageDestroy(im);
  return gdNumFailures();
}
