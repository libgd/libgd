#include "gd.h"
#include <stdio.h>
#include <stdlib.h>

void save_png(gdSurfacePtr surface, const char *filename)
{
	FILE *fp;
	fp = fopen(filename, "wb");
	if (!fp) {
		fprintf(stderr, "Can't save png image %s\n", filename);
		return;
	}
	gdSurfacePng(surface, fp);
	fclose(fp);
}

int main()
{
	gdSurface *surface;

    surface = gdSurfaceCreate(500, 500, GD_SURFACE_ARGB32);

    int x = 100;
    for (int y = 100; y < 150; y++) {
        unsigned int* img = (unsigned int*)(surface->data + surface->stride * y);
        for (int x = 100; x < 150; x++) {
            img[x] = 0x80FF0000;
        }
    }
	if (!surface) {
		fprintf(stderr, "Can't create 400x400 surface\n");
		return 1;
	}

	save_png(surface, "surface1.png");

	gdSurfaceDestroy(surface);
	return 0;
}
