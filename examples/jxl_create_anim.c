#include <gd.h>
#include "gdfonts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 320
#define HEIGHT 180
#define FRAMES 48

static int clamp_u8(int v) {
	if (v < 0) {
		return 0;
	}
	if (v > 255) {
		return 255;
	}
	return v;
}

static void draw_gradient(gdImagePtr im, int frame) {
	int y, x;

	for (y = 0; y < HEIGHT; y++) {
		int r0 = 18 + y / 5;
		int g0 = 30 + y / 4;
		int b0 = 70 + y / 2;
		for (x = 0; x < WIDTH; x++) {
			int glow = (x + frame * 7) % WIDTH;
			int wave = glow < WIDTH / 2 ? glow : WIDTH - glow;
			int r = r0 + wave / 9;
			int g = g0 + wave / 14;
			int b = b0 + wave / 18;
			gdImageSetPixel(im, x, y,
							gdTrueColor(clamp_u8(r), clamp_u8(g),
										clamp_u8(b)));
		}
	}
}

static void draw_frame(gdImagePtr im, int frame) {
	int cx = 36 + (frame * (WIDTH - 72)) / (FRAMES - 1);
	int cy = HEIGHT / 2 + ((frame % 16) - 8) * 3;
	int ring, stripe;
	int white = gdTrueColorAlpha(255, 255, 255, 18);
	int gold = gdTrueColorAlpha(255, 212, 90, 28);
	int cyan = gdTrueColorAlpha(80, 220, 255, 36);
	int black = gdTrueColorAlpha(0, 0, 0, 72);

	gdImageAlphaBlending(im, 1);
	gdImageSaveAlpha(im, 1);
	draw_gradient(im, frame);

	for (stripe = 0; stripe < WIDTH; stripe += 18) {
		int x = (stripe + frame * 5) % WIDTH;
		gdImageLine(im, x, 0, x - 80, HEIGHT - 1, gdTrueColorAlpha(255, 255, 255, 95));
	}

	gdImageFilledEllipse(im, cx, cy, 72, 72, cyan);
	gdImageFilledEllipse(im, WIDTH - cx, HEIGHT - cy, 54, 54, gold);

	for (ring = 0; ring < 5; ring++) {
		int d = 28 + ring * 18 + (frame % 6) * 2;
		gdImageEllipse(im, cx, cy, d, d, white);
		gdImageEllipse(im, WIDTH - cx, HEIGHT - cy, d + 12, d + 12, white);
	}

	gdImageFilledRectangle(im, 0, HEIGHT - 28, WIDTH - 1, HEIGHT - 1, black);
	gdImageString(im, gdFontGetSmall(), 12, HEIGHT - 20,
				  (unsigned char *)"libgd JPEG XL animation", white);
}

static gdImagePtr make_frame(int frame) {
	gdImagePtr im = gdImageCreateTrueColor(WIDTH, HEIGHT);
	if (im == NULL) {
		return NULL;
	}
	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, 1);
	draw_frame(im, frame);
	return im;
}

int main(int argc, char **argv) {
	FILE *out;
	gdJxlAnimPtr anim;
	int lossless = 0;
	float distance = 1.0f;
	int i;

	if (argc < 2 || argc > 4) {
		fprintf(stderr,
				"Usage: %s output.jxl [lossless|lossy] [distance]\n",
				argv[0]);
		return 1;
	}

	if (argc >= 3) {
		lossless = strcmp(argv[2], "lossless") == 0;
	}
	if (argc >= 4) {
		distance = (float)atof(argv[3]);
	}

	out = fopen(argv[1], "wb");
	if (out == NULL) {
		fprintf(stderr, "cannot create %s\n", argv[1]);
		return 1;
	}

	anim = gdImageJxlAnimBegin(out, WIDTH, HEIGHT, lossless, distance, 7);
	if (anim == NULL) {
		fprintf(stderr, "cannot create JXL animation writer\n");
		fclose(out);
		return 1;
	}

	for (i = 0; i < FRAMES; i++) {
		gdImagePtr frame = make_frame(i);
		if (frame == NULL) {
			fprintf(stderr, "cannot create frame %d\n", i);
			gdImageJxlAnimEnd(anim);
			fclose(out);
			return 1;
		}
		if (!gdImageJxlAnimAddFrame(anim, frame, 40)) {
			fprintf(stderr, "cannot add frame %d\n", i);
			gdImageDestroy(frame);
			gdImageJxlAnimEnd(anim);
			fclose(out);
			return 1;
		}
		gdImageDestroy(frame);
	}

	if (!gdImageJxlAnimEnd(anim)) {
		fprintf(stderr, "cannot finalize JXL animation\n");
		fclose(out);
		return 1;
	}

	fclose(out);
	printf("wrote %s: %d frames, %dx%d, %s\n", argv[1], FRAMES, WIDTH,
		   HEIGHT, lossless ? "lossless" : "lossy");
	return 0;
}
