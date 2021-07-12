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
#include "gd_array.h"

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

    if (type >= GD_SURFACE_COUNT) {
        return NULL;
    }
    if (checkOverflowAndType(width, height, type)) {
        return NULL;
    }
    surface = gdMalloc(sizeof(gdSurface));
	if (!surface) {
		return NULL;
	}

    surface->data = gdMalloc((size_t)(width * height * 4));
	if (!surface->data) {
        gdFree(surface);
		return NULL;
	}
    memset(surface->data, 0, (size_t)(width * height * 4));
    surface->type = type;
    surface->ref = 1;
    surface->gdOwned = 1;
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
    surface->type = type;
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

BGD_DECLARE(unsigned char *) gdSurfaceGetData(const gdSurfacePtr surface)
{
    if(surface==NULL) {
        return NULL;
    }
    return surface->data;
}

BGD_DECLARE(gdSurfaceType) gdSurfaceGetType(const gdSurfacePtr surface)
{
    if(surface==NULL) {
        return GD_SURFACE_NONE;
    }
    return surface->type;
}

BGD_DECLARE(int) gdSurfaceGetWidth(const gdSurfacePtr surface)
{
    return surface->width;
}

BGD_DECLARE(int) gdSurfaceGetHeight(const gdSurfacePtr surface)
{
    return surface->height;
}

BGD_DECLARE(int) gdSurfaceGetStride(const gdSurfacePtr surface)
{
    return surface->stride;
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
