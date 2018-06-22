#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int error = 0;

	im = gdImageCreateTrueColor(141,200);
	if (im == NULL) {
		gdTestErrorMsg("image creation failed.\n");
		goto exit;
	}

	gdImageAlphaBlending(im, gdEffectNormal);
	gdImageFilledRectangle(im, 0, 0, 140, 199, gdTrueColorAlpha(0,0,0,127));

	gdImageAlphaBlending(im, gdEffectAlphaBlend);
	gdImageFilledEllipse(im, 70, 70, 45, 60, gdTrueColorAlpha(255,0,0,40));
	gdImageFilledEllipse(im, 90, 90, 60, 30, gdTrueColorAlpha(255,0,0,40));
	gdImageSaveAlpha(im, 1);

	if (!gdAssertImageEqualsToFile("gdimagefilledellipse/github_bug_00238_exp.png", im)) {
		error = 1;
	}

	if (im != NULL) {
		gdImageDestroy(im);
	}

exit:
	return error;
}
