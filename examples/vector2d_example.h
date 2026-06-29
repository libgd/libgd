#ifndef GD_VECTOR2D_EXAMPLE_H
#define GD_VECTOR2D_EXAMPLE_H

#include "gd_vector2d.h"
#include <stdio.h>

static inline int vector2d_save_png(gdImagePtr image, const char *filename)
{
	FILE *file = fopen(filename, "wb");
	if (!file) {
		fprintf(stderr, "Cannot write %s\n", filename);
		return 0;
	}
	gdImagePng(image, file);
	fclose(file);
	return 1;
}

static inline gdImagePtr vector2d_create_image(int width, int height, int color)
{
	gdImagePtr image = gdImageCreateTrueColor(width, height);
	if (image) {
		/* Store the requested alpha verbatim while initializing the canvas. */
		gdImageAlphaBlending(image, 0);
		gdImageFilledRectangle(image, 0, 0, width - 1, height - 1, color);
		gdImageAlphaBlending(image, 1);
		gdImageSaveAlpha(image, 1);
	}
	return image;
}

static inline gdImagePtr vector2d_create_checker(int width, int height)
{
	gdImagePtr image = vector2d_create_image(width, height,
		gdTrueColorAlpha(240, 240, 240, 0));
	if (image) {
		int dark = gdTrueColorAlpha(55, 95, 150, 0);
		int light = gdTrueColorAlpha(245, 170, 65, 0);
		int size = width / 8 > 4 ? width / 8 : 4;
		int x, y;
		for (y = 0; y < height; y += size) {
			for (x = 0; x < width; x += size) {
				gdImageFilledRectangle(image, x, y, x + size - 1, y + size - 1,
					((x / size + y / size) & 1) ? dark : light);
			}
		}
	}
	return image;
}

#endif
