#include "gd.h"
#include "gdtest.h"
#include <string.h>

static gdImagePtr make_frame(int color)
{
	gdImagePtr im = gdImageCreateTrueColor(4, 4);
	int x, y;

	if (im == NULL) {
		return NULL;
	}
	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, 1);
	for (y = 0; y < 4; y++) {
		for (x = 0; x < 4; x++) {
			gdImageSetPixel(im, x, y, color);
		}
	}
	return im;
}

int main()
{
	gdWebpWriteOptions options;
	gdWebpWritePtr writer;
	gdWebpReadPtr reader;
	gdWebpInfo info;
	gdWebpFrameInfo frameInfo;
	gdImagePtr red, blue, image, clone;
	void *data;
	int size = 0;

	memset(&options, 0, sizeof(options));
	options.canvasWidth = 4;
	options.canvasHeight = 4;
	options.loopCount = 3;
	options.quality = gdWebpLossless;

	red = make_frame(gdTrueColorAlpha(255, 0, 0, gdAlphaOpaque));
	blue = make_frame(gdTrueColorAlpha(0, 0, 255, gdAlphaOpaque));
	if (red == NULL || blue == NULL) {
		return 1;
	}

	writer = gdWebpWriteOpenPtr(&options);
	gdTestAssert(writer != NULL);
	gdTestAssert(gdWebpWriteAddImage(writer, red, 120));
	gdTestAssert(gdWebpWriteAddImage(writer, blue, 80));
	data = gdWebpWritePtrFinish(writer, &size);
	gdTestAssert(data != NULL);
	gdTestAssert(size > 0);

	gdTestAssert(gdWebpIsAnimatedPtr(size, data) == 1);
	reader = gdWebpReadOpenPtr(size, data);
	gdTestAssert(reader != NULL);
	gdTestAssert(gdWebpReadGetInfo(reader, &info));
	gdTestAssert(info.width == 4);
	gdTestAssert(info.height == 4);
	gdTestAssert(info.frameCount == 2);
	gdTestAssert(info.loopCount == 3);

	gdTestAssert(gdWebpReadNextFrame(reader, &frameInfo, &image) == 1);
	gdTestAssert(frameInfo.frameIndex == 0);
	gdTestAssert(frameInfo.duration == 120);
	gdTestAssert(gdImageSX(image) == 4);
	gdTestAssert(gdImageSY(image) == 4);
	gdTestAssert(gdWebpReadNextFrame(reader, &frameInfo, &image) == 1);
	gdTestAssert(frameInfo.frameIndex == 1);
	gdTestAssert(frameInfo.duration == 80);
	gdTestAssert(gdWebpReadNextFrame(reader, &frameInfo, &image) == 0);
	gdWebpReadClose(reader);

	reader = gdWebpReadOpenPtr(size, data);
	gdTestAssert(reader != NULL);
	gdTestAssert(gdWebpReadNextImage(reader, &frameInfo, &image) == 1);
	clone = gdWebpReadCloneImage(reader);
	gdTestAssert(clone != NULL);
	gdTestAssert(gdImageGetPixel(clone, 0, 0) == gdImageGetPixel(image, 0, 0));
	gdTestAssert(gdWebpReadNextImage(reader, &frameInfo, &image) == 1);
	gdTestAssert(gdImageGetPixel(clone, 0, 0) != gdImageGetPixel(image, 0, 0));
	gdImageDestroy(clone);
	gdWebpReadClose(reader);

	gdTestAssert(gdWebpIsAnimatedPtr(4, "nope") == -1);

	gdFree(data);
	gdImageDestroy(red);
	gdImageDestroy(blue);
	return 0;
}
