#include "gd.h"
#include "gdtest.h"

#include <stdio.h>
#include <stdlib.h>

static char *fixture_path(const char *file)
{
	return gdTestFilePathX("gif", "animated_gif_reader", file, NULL);
}

static FILE *fixture_open(const char *file)
{
	char *path;
	FILE *fp;

	path = fixture_path(file);
	if (path == NULL) {
		return NULL;
	}
	fp = fopen(path, "rb");
	free(path);
	return fp;
}

static void *fixture_data(const char *file, int *size)
{
	char *path;
	FILE *fp;
	void *data = NULL;

	*size = 0;
	path = fixture_path(file);
	if (path == NULL) {
		return NULL;
	}
	fp = fopen(path, "rb");
	free(path);
	if (fp == NULL) {
		return NULL;
	}
	fseek(fp, 0, SEEK_END);
	*size = (int) ftell(fp);
	fseek(fp, 0, SEEK_SET);
	data = malloc(*size);
	if (data == NULL || (int) fread(data, 1, *size, fp) != *size) {
		free(data);
		data = NULL;
		*size = 0;
	}
	fclose(fp);
	return data;
}

static int color_is(gdImagePtr im, int x, int y, int r, int g, int b)
{
	int c = gdImageGetPixel(im, x, y);

	return gdTrueColorGetRed(c) == r &&
	       gdTrueColorGetGreen(c) == g &&
	       gdTrueColorGetBlue(c) == b;
}

static void test_probe(void)
{
	FILE *fp;
	long pos;
	void *data = NULL;
	gdIOCtxPtr ctx;
	int size = 0;

	fp = fixture_open("static_4x4_red.gif");
	gdTestAssert(fp != NULL);
	if (fp != NULL) {
		pos = ftell(fp);
		gdTestAssertMsg(gdGifIsAnimated(fp) == 0, "static GIF should not be animated");
		gdTestAssertMsg(ftell(fp) == pos, "gdGifIsAnimated should restore FILE position");
		fclose(fp);
	}

	fp = fixture_open("anim_3frame_rgb.gif");
	gdTestAssert(fp != NULL);
	if (fp != NULL) {
		pos = ftell(fp);
		gdTestAssertMsg(gdGifIsAnimated(fp) == 1, "3-frame GIF should be animated");
		gdTestAssertMsg(ftell(fp) == pos, "gdGifIsAnimated should restore animated FILE position");
		fclose(fp);
	}

	fp = fixture_open("bad_magic.gif");
	gdTestAssert(fp != NULL);
	if (fp != NULL) {
		gdTestAssertMsg(gdGifIsAnimated(fp) == -1, "invalid GIF should fail probe");
		fclose(fp);
	}

	data = fixture_data("anim_2frame.gif", &size);
	gdTestAssert(data != NULL);
	if (data != NULL) {
		gdTestAssertMsg(gdGifIsAnimatedPtr(size, data) == 1, "ptr GIF should be animated");
		ctx = gdNewDynamicCtxEx(size, data, 0);
		gdTestAssert(ctx != NULL);
		if (ctx != NULL) {
			pos = ctx->tell(ctx);
			gdTestAssertMsg(gdGifIsAnimatedCtx(ctx) == 1, "ctx GIF should be animated");
			gdTestAssertMsg(ctx->tell(ctx) == pos, "gdGifIsAnimatedCtx should restore position");
			ctx->gd_free(ctx);
		}
		free(data);
	}

	data = fixture_data("static_4x4_red.gif", &size);
	gdTestAssert(data != NULL);
	if (data != NULL) {
		gdTestAssertMsg(gdGifIsAnimatedPtr(size, data) == 0, "static ptr GIF should not be animated");
		ctx = gdNewDynamicCtxEx(size, data, 0);
		gdTestAssert(ctx != NULL);
		if (ctx != NULL) {
			gdTestAssertMsg(gdGifIsAnimatedCtx(ctx) == 0, "static ctx GIF should not be animated");
			ctx->gd_free(ctx);
		}
		free(data);
	}

	data = fixture_data("bad_magic.gif", &size);
	gdTestAssert(data != NULL);
	if (data != NULL) {
		gdTestAssertMsg(gdGifIsAnimatedPtr(size, data) == -1, "invalid ptr GIF should fail probe");
		ctx = gdNewDynamicCtxEx(size, data, 0);
		gdTestAssert(ctx != NULL);
		if (ctx != NULL) {
			pos = ctx->tell(ctx);
			gdTestAssertMsg(gdGifIsAnimatedCtx(ctx) == -1, "invalid ctx GIF should fail probe");
			gdTestAssertMsg(ctx->tell(ctx) == pos, "invalid ctx probe should restore position");
			ctx->gd_free(ctx);
		}
		free(data);
	}
}

static void test_raw_iterator(void)
{
	FILE *fp;
	gdGifReadPtr gif;
	gdGifInfo info;
	gdGifFrameInfo frameInfo;
	gdImagePtr frame;
	int count = 0;
	int delay = 0;

	fp = fixture_open("anim_3frame_rgb.gif");
	gdTestAssert(fp != NULL);
	if (fp == NULL) {
		return;
	}

	gif = gdGifReadOpen(fp);
	gdTestAssert(gif != NULL);
	if (gif == NULL) {
		fclose(fp);
		return;
	}

	gdTestAssert(gdGifReadGetInfo(gif, &info));
	gdTestAssertMsg(info.width == 8 && info.height == 8, "unexpected logical screen size");
	gdTestAssertMsg(info.loopCount == 0, "expected infinite Netscape loop count");

	while (gdGifReadNextFrame(gif, &frameInfo, &frame) == 1) {
		gdTestAssert(frame != NULL);
		gdTestAssertMsg(frameInfo.frameIndex == count, "unexpected frame index");
		gdTestAssertMsg(frameInfo.width == 8 && frameInfo.height == 8, "unexpected frame size");
		gdTestAssertMsg(frameInfo.delay == 50, "unexpected frame delay");
		delay += frameInfo.delay;
		count++;
	}

	gdTestAssertMsg(count == 3, "expected 3 frames, got %d", count);
	gdTestAssertMsg(delay == 150, "expected total delay 150, got %d", delay);
	gdGifReadClose(gif);
	fclose(fp);
}

static void test_local_color_table(void)
{
	FILE *fp;
	gdGifReadPtr gif;
	gdGifFrameInfo info;
	gdImagePtr frame;

	fp = fixture_open("local_ct.gif");
	gdTestAssert(fp != NULL);
	if (fp == NULL) {
		return;
	}
	gif = gdGifReadOpen(fp);
	gdTestAssert(gif != NULL);
	if (gif == NULL) {
		fclose(fp);
		return;
	}

	gdTestAssert(gdGifReadNextFrame(gif, &info, &frame) == 1);
	gdTestAssertMsg(info.localColorTable == 0, "first frame should use global color table");
	gdTestAssert(gdGifReadNextFrame(gif, &info, &frame) == 1);
	gdTestAssertMsg(info.localColorTable == 1, "second frame should use local color table");
	gdTestAssertMsg(frame->red[1] == 255 && frame->green[1] == 128 && frame->blue[1] == 0,
	                "local palette color should be orange");

	gdGifReadClose(gif);
	fclose(fp);
}

static void test_composited_iterator(void)
{
	FILE *fp;
	gdGifReadPtr gif;
	gdGifFrameInfo info;
	gdImagePtr image;

	fp = fixture_open("small_frame_big_canvas.gif");
	gdTestAssert(fp != NULL);
	if (fp == NULL) {
		return;
	}
	gif = gdGifReadOpen(fp);
	gdTestAssert(gif != NULL);
	if (gif == NULL) {
		fclose(fp);
		return;
	}

	gdTestAssert(gdGifReadNextImage(gif, &info, &image) == 1);
	gdTestAssertMsg(image->sx == 16 && image->sy == 16, "composited image should use canvas size");
	gdTestAssertMsg(color_is(image, 0, 0, 200, 200, 200), "canvas background should be preserved");
	gdTestAssertMsg(color_is(image, 6, 6, 255, 0, 0), "frame pixel should be composited at offset");
	gdGifReadClose(gif);
	fclose(fp);

	fp = fixture_open("dispose_background.gif");
	gdTestAssert(fp != NULL);
	if (fp == NULL) {
		return;
	}
	gif = gdGifReadOpen(fp);
	gdTestAssert(gif != NULL);
	if (gif == NULL) {
		fclose(fp);
		return;
	}

	gdTestAssert(gdGifReadNextImage(gif, &info, &image) == 1);
	gdTestAssertMsg(color_is(image, 0, 0, 255, 0, 0), "first frame should draw red left half");
	gdTestAssert(gdGifReadNextImage(gif, &info, &image) == 1);
	gdTestAssertMsg(color_is(image, 0, 0, 255, 255, 255), "previous frame should restore to background");
	gdTestAssertMsg(color_is(image, 4, 0, 0, 0, 255), "second frame should draw blue right half");

	gdGifReadClose(gif);
	fclose(fp);
}

static void test_legacy_first_frame(void)
{
	FILE *fp;
	gdImagePtr im;

	fp = fixture_open("small_frame_big_canvas.gif");
	gdTestAssert(fp != NULL);
	if (fp == NULL) {
		return;
	}
	im = gdImageCreateFromGif(fp);
	gdTestAssert(im != NULL);
	if (im != NULL) {
		gdTestAssertMsg(im->sx == 4 && im->sy == 4, "legacy GIF reader should keep raw first-frame size");
		gdImageDestroy(im);
	}
	fclose(fp);
}

int main()
{
	test_probe();
	test_raw_iterator();
	test_local_color_table();
	test_composited_iterator();
	test_legacy_first_frame();

	return gdNumFailures();
}
