#include "gd.h"
#include "gdtest.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	const char *dir;
	const char *file;
	int width;
	int height;
	int frames;
	int loopCount;
	int totalDelay;
} gif_case_t;

static const gif_case_t valid_cases[] = {
	{"valid", "static_4x4_red.gif", 4, 4, 1, 1, 0},
	{"valid", "static_8x8_palette.gif", 8, 8, 1, 1, 0},
	{"valid", "static_256colors.gif", 16, 16, 1, 1, 0},
	{"valid", "static_interlaced.gif", 8, 8, 1, 1, 0},
	{"valid", "anim_2frame.gif", 8, 8, 2, 0, 100},
	{"valid", "anim_3frame_rgb.gif", 8, 8, 3, 0, 150},
	{"valid", "anim_10frame.gif", 8, 8, 10, 0, 200},
	{"valid", "dispose_unspecified.gif", 8, 8, 2, 0, 100},
	{"valid", "dispose_none.gif", 8, 8, 2, 0, 100},
	{"valid", "dispose_background.gif", 8, 8, 2, 0, 100},
	{"valid", "dispose_previous.gif", 8, 8, 2, 0, 100},
	{"valid", "transparent_bg.gif", 8, 8, 1, 1, 0},
	{"valid", "transparent_frame.gif", 8, 8, 2, 0, 100},
	{"valid", "delay_0.gif", 4, 4, 2, 0, 0},
	{"valid", "delay_10ms.gif", 4, 4, 2, 0, 2},
	{"valid", "delay_1s.gif", 4, 4, 2, 0, 200},
	{"valid", "variable_delay.gif", 4, 4, 4, 0, 360},
	{"valid", "loop_infinite.gif", 4, 4, 2, 0, 100},
	{"valid", "loop_once.gif", 4, 4, 2, 1, 100},
	{"valid", "loop_3.gif", 4, 4, 2, 3, 100},
	{"valid", "no_loop_ext.gif", 4, 4, 2, 1, 100},
	{"valid", "global_ct_only.gif", 4, 4, 2, 0, 100},
	{"valid", "local_ct.gif", 4, 4, 2, 0, 100},
	{"valid", "mixed_ct.gif", 4, 4, 3, 0, 150},
	{"valid", "small_frame_big_canvas.gif", 16, 16, 1, 1, 0},
	{"valid", "overlapping_frames.gif", 16, 16, 2, 0, 100},
	{"valid", "1x1.gif", 1, 1, 1, 1, 0},
	{"valid", "2color.gif", 4, 4, 1, 1, 0},
	{"edge-cases", "gif87a.gif", 4, 4, 1, 1, 0},
	{"edge-cases", "comment_ext.gif", 4, 4, 1, 1, 0},
	{"edge-cases", "plain_text_ext.gif", 64, 16, 1, 1, 0},
	{"edge-cases", "large_palette_small_image.gif", 4, 4, 1, 1, 0}
};

static const char *invalid_cases[] = {
	"bad_magic.gif",
	"truncated_header.gif",
	"truncated_lzw.gif",
	"empty.gif",
	"no_trailer.gif",
	"bad_lzw_code.gif",
	"zero_dimensions.gif"
};

static char *conformance_path(const char *dir, const char *file)
{
	return gdTestFilePathX("gif", "gif-conformance", dir, file, NULL);
}

static int pixel_is(gdImagePtr im, int x, int y, int r, int g, int b)
{
	int c = gdImageGetPixel(im, x, y);

	return gdTrueColorGetRed(c) == r &&
	       gdTrueColorGetGreen(c) == g &&
	       gdTrueColorGetBlue(c) == b &&
	       gdTrueColorGetAlpha(c) == gdAlphaOpaque;
}

static int pixel_is_transparent(gdImagePtr im, int x, int y)
{
	return gdTrueColorGetAlpha(gdImageGetPixel(im, x, y)) == gdAlphaTransparent;
}

static void check_pixels(const char *file, int frame, gdImagePtr im)
{
	if (strcmp(file, "static_4x4_red.gif") == 0) {
		gdTestAssertMsg(pixel_is(im, 0, 0, 255, 0, 0), "%s should be red", file);
	} else if (strcmp(file, "static_8x8_palette.gif") == 0) {
		gdTestAssertMsg(pixel_is(im, 1, 0, 0, 255, 0), "%s palette pixel mismatch", file);
		gdTestAssertMsg(pixel_is(im, 7, 0, 255, 255, 255), "%s palette pixel mismatch", file);
	} else if (strcmp(file, "static_256colors.gif") == 0) {
		gdTestAssertMsg(pixel_is(im, 15, 15, 249, 37, 77), "%s 256-color pixel mismatch", file);
	} else if (strcmp(file, "static_interlaced.gif") == 0) {
		gdTestAssertMsg(pixel_is(im, 0, 3, 0, 0, 255), "%s interlaced pixel mismatch", file);
	} else if (strcmp(file, "anim_3frame_rgb.gif") == 0) {
		if (frame == 0) gdTestAssertMsg(pixel_is(im, 0, 0, 255, 0, 0), "%s frame 0 mismatch", file);
		if (frame == 1) gdTestAssertMsg(pixel_is(im, 0, 0, 0, 255, 0), "%s frame 1 mismatch", file);
		if (frame == 2) gdTestAssertMsg(pixel_is(im, 0, 0, 0, 0, 255), "%s frame 2 mismatch", file);
	} else if (strcmp(file, "dispose_none.gif") == 0 && frame == 1) {
		gdTestAssertMsg(pixel_is(im, 0, 0, 255, 0, 0), "%s should preserve previous left half", file);
		gdTestAssertMsg(pixel_is(im, 4, 0, 0, 0, 255), "%s should draw right half", file);
	} else if (strcmp(file, "dispose_background.gif") == 0 && frame == 1) {
		gdTestAssertMsg(pixel_is(im, 0, 0, 255, 255, 255), "%s should restore background", file);
		gdTestAssertMsg(pixel_is(im, 4, 0, 0, 0, 255), "%s should draw right half", file);
	} else if (strcmp(file, "dispose_previous.gif") == 0 && frame == 1) {
		gdTestAssertMsg(pixel_is(im, 0, 0, 255, 255, 255), "%s should restore previous canvas", file);
		gdTestAssertMsg(pixel_is(im, 4, 0, 0, 0, 255), "%s should draw right half", file);
	} else if (strcmp(file, "transparent_bg.gif") == 0) {
		gdTestAssertMsg(pixel_is_transparent(im, 0, 0), "%s transparent background mismatch", file);
		gdTestAssertMsg(pixel_is(im, 1, 0, 255, 0, 0), "%s visible pixel mismatch", file);
	} else if (strcmp(file, "small_frame_big_canvas.gif") == 0) {
		gdTestAssertMsg(pixel_is(im, 0, 0, 200, 200, 200), "%s background mismatch", file);
		gdTestAssertMsg(pixel_is(im, 6, 6, 255, 0, 0), "%s frame offset mismatch", file);
	} else if (strcmp(file, "overlapping_frames.gif") == 0 && frame == 1) {
		gdTestAssertMsg(pixel_is(im, 0, 0, 255, 0, 0), "%s preserved area mismatch", file);
		gdTestAssertMsg(pixel_is(im, 4, 4, 0, 0, 255), "%s overlap area mismatch", file);
	} else if (strcmp(file, "local_ct.gif") == 0 && frame == 1) {
		gdTestAssertMsg(pixel_is(im, 0, 0, 255, 128, 0), "%s local color table mismatch", file);
	} else if (strcmp(file, "mixed_ct.gif") == 0 && frame == 1) {
		gdTestAssertMsg(pixel_is(im, 0, 0, 255, 128, 0), "%s mixed local color mismatch", file);
	} else if (strcmp(file, "mixed_ct.gif") == 0 && frame == 2) {
		gdTestAssertMsg(pixel_is(im, 0, 0, 0, 0, 255), "%s mixed global color mismatch", file);
	} else if (strcmp(file, "2color.gif") == 0) {
		gdTestAssertMsg(pixel_is(im, 0, 0, 0, 0, 0), "%s first pixel mismatch", file);
		gdTestAssertMsg(pixel_is(im, 1, 0, 255, 255, 255), "%s second pixel mismatch", file);
	}
}

static int read_all_frames(const gif_case_t *testCase)
{
	char *path = conformance_path(testCase->dir, testCase->file);
	FILE *fp;
	gdGifReadPtr gif;
	gdGifInfo info;
	gdGifFrameInfo frameInfo;
	gdImagePtr im;
	int frames = 0;
	int totalDelay = 0;
	int result;

	fp = fopen(path, "rb");
	gdTestAssertMsg(fp != NULL, "failed to open GIF conformance file %s", path);
	if (fp == NULL) {
		free(path);
		return 0;
	}

	gif = gdGifReadOpen(fp);
	gdTestAssertMsg(gif != NULL, "failed to open valid GIF conformance file %s", path);
	if (gif == NULL) {
		fclose(fp);
		free(path);
		return 0;
	}

	gdTestAssert(gdGifReadGetInfo(gif, &info));
	gdTestAssertMsg(info.width == testCase->width && info.height == testCase->height,
	                "%s canvas mismatch: got %dx%d expected %dx%d",
	                path, info.width, info.height, testCase->width, testCase->height);
	gdTestAssertMsg(info.loopCount == testCase->loopCount,
	                "%s loop mismatch: got %d expected %d",
	                path, info.loopCount, testCase->loopCount);

	while ((result = gdGifReadNextImage(gif, &frameInfo, &im)) == 1) {
		gdTestAssertMsg(im != NULL, "%s returned NULL composited frame", path);
		gdTestAssertMsg(im->sx == testCase->width && im->sy == testCase->height,
		                "%s frame %d image size mismatch", path, frames);
		gdTestAssertMsg(frameInfo.frameIndex == frames,
		                "%s frame index mismatch: got %d expected %d",
		                path, frameInfo.frameIndex, frames);
		totalDelay += frameInfo.delay;
		check_pixels(testCase->file, frames, im);
		frames++;
	}

	gdTestAssertMsg(result == 0, "valid GIF conformance file ended with decode error: %s", path);
	gdTestAssertMsg(frames == testCase->frames,
	                "%s frame count mismatch: got %d expected %d",
	                path, frames, testCase->frames);
	gdTestAssertMsg(totalDelay == testCase->totalDelay,
	                "%s total delay mismatch: got %d expected %d",
	                path, totalDelay, testCase->totalDelay);

	gdGifReadClose(gif);
	fclose(fp);
	free(path);
	return result == 0;
}

static void test_valid_and_edge_cases(void)
{
	size_t i;

	for (i = 0; i < sizeof(valid_cases) / sizeof(valid_cases[0]); i++) {
		read_all_frames(&valid_cases[i]);
	}
}

static void test_invalid_cases(void)
{
	size_t i;

	for (i = 0; i < sizeof(invalid_cases) / sizeof(invalid_cases[0]); i++) {
		char *path = conformance_path("invalid", invalid_cases[i]);
		FILE *fp = fopen(path, "rb");
		gdGifReadPtr gif = NULL;
		int result = -1;

		gdTestAssertMsg(fp != NULL, "failed to open invalid GIF conformance file %s", path);
		if (fp != NULL) {
			gif = gdGifReadOpen(fp);
			if (gif != NULL) {
				gdGifFrameInfo frameInfo;
				gdImagePtr im;

				do {
					result = gdGifReadNextImage(gif, &frameInfo, &im);
				} while (result == 1);
				gdGifReadClose(gif);
			}
			fclose(fp);
		}

		gdTestAssertMsg(gif == NULL || result < 0,
		                "invalid GIF conformance file decoded successfully: %s",
		                path);
		free(path);
	}
}

int main(void)
{
	test_valid_and_edge_cases();
	test_invalid_cases();

	return gdNumFailures();
}
