#include "gdfonts.h"
#include <gd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 320
#define HEIGHT 180
#define FRAMES 60 /* must be even; period of all sin/cos calls */

/* ------------------------------------------------------------------ */
/* Helpers                                                              */
/* ------------------------------------------------------------------ */

static int clamp_u8(int v) { return v < 0 ? 0 : v > 255 ? 255 : v; }

/* t in [0,1), returns value in [0,1) that loops smoothly */
static double phase(int frame, double speed) {
	return fmod((double)frame / FRAMES * speed, 1.0);
}

static double tw(int frame, double speed) { /* 0..2pi periodic */
	return phase(frame, speed) * 2.0 * M_PI;
}

/* ------------------------------------------------------------------ */
/* Starfield: fixed star positions, scroll speed varies per "layer"    */
/* ------------------------------------------------------------------ */

#define NSTARS 80
typedef struct {
	float x, y, speed, brightness;
} Star;
static Star stars[NSTARS];

static void init_stars(void) {
	/* deterministic pseudo-random so the file is reproducible */
	unsigned int rng = 0xdeadbeef;
	int i;
	for (i = 0; i < NSTARS; i++) {
		rng = rng * 1664525u + 1013904223u;
		stars[i].x = (rng & 0xffff) / 65535.0f * WIDTH;
		rng = rng * 1664525u + 1013904223u;
		stars[i].y = (rng & 0xffff) / 65535.0f * HEIGHT;
		rng = rng * 1664525u + 1013904223u;
		/* three parallax layers */
		stars[i].speed = 0.3f + ((rng >> 16) % 3) * 0.5f;
		rng = rng * 1664525u + 1013904223u;
		stars[i].brightness = 0.4f + (rng & 0xff) / 255.0f * 0.6f;
	}
}

static void draw_stars(gdImagePtr im, int frame) {
	int i;
	for (i = 0; i < NSTARS; i++) {
		/* x scrolls at star's speed, wraps mod WIDTH — seamless because
		   total travel = speed * WIDTH pixels over FRAMES frames,
		   and we take mod WIDTH */
		double scroll = stars[i].speed * WIDTH * ((double)frame / FRAMES);
		int x = ((int)(stars[i].x + scroll)) % WIDTH;
		int y = (int)stars[i].y;
		int b = clamp_u8((int)(stars[i].brightness * 220));
		/* twinkle: each star has a unique phase offset */
		double twinkle = 0.7 + 0.3 * sin(tw(frame, stars[i].speed * 2.0) +
										 stars[i].x * 0.05);
		b = clamp_u8((int)(b * twinkle));
		gdImageSetPixel(im, x, y, gdTrueColorAlpha(b, b, b + 20, 0));
	}
}

/* ------------------------------------------------------------------ */
/* Plasma background                                                    */
/* ------------------------------------------------------------------ */

static void draw_plasma(gdImagePtr im, int frame) {
	double t = (double)frame / FRAMES * 2.0 * M_PI;
	int x, y;
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			double fx = x / (double)WIDTH;
			double fy = y / (double)HEIGHT;

			/* three sine waves with different axes — all 2pi periodic in t */
			double v = sin(fx * 6.0 + t);
			v += sin(fy * 4.0 + t * 1.3);
			v += sin((fx + fy) * 5.0 + t * 0.7);
			v += sin(
				sqrt((fx - 0.5 + 0.3 * sin(t)) * (fx - 0.5 + 0.3 * sin(t)) +
					 (fy - 0.5 + 0.3 * cos(t)) * (fy - 0.5 + 0.3 * cos(t))) *
				8.0);
			v = (v + 4.0) / 8.0; /* normalise to [0,1] */

			int r = clamp_u8((int)(18 + v * 40));
			int g = clamp_u8((int)(10 + v * 25));
			int b = clamp_u8((int)(60 + v * 80));
			gdImageSetPixel(im, x, y, gdTrueColor(r, g, b));
		}
	}
}

/* ------------------------------------------------------------------ */
/* Diagonal scan-line shimmer (infinite scroll, seamless)              */
/* ------------------------------------------------------------------ */

static void draw_scanlines(gdImagePtr im, int frame) {
	/* lines travel left; total shift over FRAMES = WIDTH, so mod WIDTH = 0 */
	int shift = (int)((double)frame / FRAMES * WIDTH);
	int s;
	for (s = 0; s < WIDTH + HEIGHT; s += 22) {
		int x0 = ((s - shift) % WIDTH + WIDTH) % WIDTH;
		/* draw with clipping — gdImageLine handles out-of-bounds fine */
		gdImageLine(im, x0, 0, x0 - HEIGHT, HEIGHT - 1,
					gdTrueColorAlpha(255, 255, 255, 108));
	}
}

/* ------------------------------------------------------------------ */
/* Orbiting glows                                                       */
/* ------------------------------------------------------------------ */

static void draw_orbs(gdImagePtr im, int frame) {
	double t = tw(frame, 1.0);

	/* two orbs in opposite circular orbits */
	int cx1 = (int)(WIDTH / 2 + cos(t) * (WIDTH / 2 - 50));
	int cy1 = (int)(HEIGHT / 2 + sin(t) * (HEIGHT / 2 - 30));
	int cx2 = (int)(WIDTH / 2 + cos(t + M_PI) * (WIDTH / 2 - 60));
	int cy2 = (int)(HEIGHT / 2 + sin(t + M_PI) * (HEIGHT / 2 - 25));

	/* pulse size: sin() guarantees seamless */
	int r1 = (int)(32 + 8 * sin(tw(frame, 2.0)));
	int r2 = (int)(26 + 6 * sin(tw(frame, 2.0) + M_PI));

	gdImageFilledEllipse(im, cx1, cy1, r1 * 2, r1 * 2,
						 gdTrueColorAlpha(80, 220, 255, 38));
	gdImageFilledEllipse(im, cx2, cy2, r2 * 2, r2 * 2,
						 gdTrueColorAlpha(255, 200, 80, 42));

	/* concentric rings that expand outward and fade — old-school */
	int ring;
	for (ring = 0; ring < 4; ring++) {
		double rphase = fmod(tw(frame, 1.5) / (2 * M_PI) + ring * 0.25, 1.0);
		int rd = (int)(rphase * 70);
		int alpha = clamp_u8((int)((1.0 - rphase) * 200));
		int col = gdTrueColorAlpha(180, 240, 255, 127 - alpha / 2);
		gdImageEllipse(im, cx1, cy1, rd * 2, rd * 2, col);
		gdImageEllipse(im, cx2, cy2, rd * 2 + 10, rd * 2 + 10, col);
	}
}

/* ------------------------------------------------------------------ */
/* HUD bar + scrolling text                                             */
/* ------------------------------------------------------------------ */

#define TICKER                                                                 \
	"  libgd JPEG XL animation  \xb7  seamless infinite loop  \xb7  plasma + " \
	"starfield + orbs  \xb7  "

static void draw_hud(gdImagePtr im, int frame) {
	/* semi-transparent bar */
	gdImageFilledRectangle(im, 0, HEIGHT - 26, WIDTH - 1, HEIGHT - 1,
						   gdTrueColorAlpha(0, 0, 0, 80));

	/* scrolling ticker: pixel offset increases monotonically mod total_width */
	int char_w = gdFontGetSmall()->w;
	int ticker_len = (int)strlen(TICKER);
	int total_px = ticker_len * char_w;
	/* scroll exactly total_px pixels over (ticker_len) frames so it tiles */
	int offset = (int)((double)frame / FRAMES * total_px) % total_px;

	/* draw twice to cover the seam */
	int gx;
	for (gx = -offset; gx < WIDTH; gx += total_px) {
		gdImageString(im, gdFontGetSmall(), gx, HEIGHT - 18,
					  (unsigned char *)TICKER,
					  gdTrueColorAlpha(180, 230, 255, 20));
	}
}

/* ------------------------------------------------------------------ */
/* Assemble one frame                                                   */
/* ------------------------------------------------------------------ */

static gdImagePtr make_frame(int frame) {
	gdImagePtr im = gdImageCreateTrueColor(WIDTH, HEIGHT);
	if (!im)
		return NULL;

	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, 1);

	draw_plasma(im, frame);

	gdImageAlphaBlending(im, 1);

	draw_stars(im, frame);
	draw_scanlines(im, frame);
	draw_orbs(im, frame);
	draw_hud(im, frame);

	return im;
}

/* ------------------------------------------------------------------ */
/* main                                                                 */
/* ------------------------------------------------------------------ */

int main(int argc, char **argv) {
	if (argc < 2 || argc > 4) {
		fprintf(stderr, "Usage: %s output.jxl [lossless|lossy] [distance]\n",
				argv[0]);
		return 1;
	}

	int lossless = argc >= 3 && strcmp(argv[2], "lossless") == 0;
	float distance = argc >= 4 ? (float)atof(argv[3]) : 1.0f;

	init_stars();

	FILE *out = fopen(argv[1], "wb");
	if (!out) {
		fprintf(stderr, "cannot create %s\n", argv[1]);
		return 1;
	}

	gdJxlAnimPtr anim =
		gdImageJxlAnimBegin(out, WIDTH, HEIGHT, lossless, distance, 7);
	if (!anim) {
		fprintf(stderr, "cannot create JXL animation writer\n");
		fclose(out);
		return 1;
	}

	int i;
	for (i = 0; i < FRAMES; i++) {
		gdImagePtr frame = make_frame(i);
		if (!frame) {
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
		fprintf(stderr, "\rframe %d/%d", i + 1, FRAMES);
	}
	fprintf(stderr, "\n");

	if (!gdImageJxlAnimEnd(anim)) {
		fprintf(stderr, "cannot finalize JXL animation\n");
		fclose(out);
		return 1;
	}

	fclose(out);
	printf("wrote %s: %d frames, %dx%d, %s distance=%.2f\n", argv[1], FRAMES,
		   WIDTH, HEIGHT, lossless ? "lossless" : "lossy", distance);
	return 0;
}