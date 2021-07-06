#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd_intern.h"
#include "gdhelpers.h"
#include "gd_errors.h"

#include "gd_surface.h"

static int checkOverflowAndType(int width, int height, unsigned type) {
    if (type > GD_SURFACE_COUNT) {
        gd_error("gd-png error: invalid gdSurface type\n");
        return 1;
    }
	if (overflow2(width, height)) {
		return 1;
	}
	if (overflow2(sizeof (unsigned char *), height)) {
		return 1;
	}
	if (overflow2(sizeof (unsigned char), width)) {
		return 1;
	}
    return 0;
}

BGD_DECLARE(gdSurfacePtr) gdSurfaceCreate(int width, int height, unsigned int type)
{
    gdSurfacePtr surface;

    if (checkOverflowAndType(width, height, type)) {
        return NULL;
    }
    surface = gdMalloc(sizeof(gdSurface));
	if (!surface) {
		return NULL;
	}
    surface->ref = 1;
    surface->gdOwned = 1;
    surface->data = gdMalloc((size_t)(width * height * 4));
	if (!surface->data) {
        gdFree(surface);
		return NULL;
	}
    memset(surface->data, 0, (size_t)(width * height * 4));
    surface->width = width;
    surface->height = height;
    surface->stride = width * 4;
    return surface;
}

BGD_DECLARE(gdSurfacePtr) gdSurfaceCreateForData(unsigned char* data, int width, int height, int stride, unsigned int type)
{
    gdSurfacePtr surface;

    if (!data) {
        return NULL;
    }
    if (checkOverflowAndType(width, height, type)) {
        return NULL;
    }

    surface = gdMalloc(sizeof(gdSurface));
	if (!surface) {
		return NULL;
	}
    surface->ref = 1;
    surface->gdOwned = 0;
    surface->data = data;
    surface->width = width;
    surface->height = height;
    surface->stride = stride;
    return surface;
}

BGD_DECLARE(gdSurfacePtr) gdSurfaceAddRef(gdSurfacePtr surface)
{
    if(surface==NULL) {
        return NULL;
    }
    surface->ref++;
    return surface;
}


BGD_DECLARE(void) gdSurfaceDestroy (gdSurfacePtr surface)
{
    if (!surface) {
        return;
    }
    if (--surface->ref == 0) {
        if (surface->gdOwned) {
            gdFree(surface->data);
        }
    }
}
