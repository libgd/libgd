/*
   * gd_security.c
   *
   * Implements buffer overflow check routines.
   *
   * Written 2004, Phil Knirsch.
   * Based on netpbm fixes by Alan Cox.
   *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "gd.h"
#include "gd_errors.h"

int overflow2(int a, int b)
{
	if(a <= 0 || b <= 0) {
		gd_error_ex(GD_WARNING, "one parameter to a memory allocation multiplication is negative or zero, failing operation gracefully\n");
		return 1;
	}
	if(a > INT_MAX / b) {
		gd_error_ex(GD_WARNING, "product of memory allocation multiplication would exceed INT_MAX, failing operation gracefully\n");
		return 1;
	}
	return 0;
}

int overflowCopyRectBounds(
	const gdImagePtr dst,
	const gdImagePtr src,
	int dstX, int dstY,
	int srcX, int srcY,
	int w, int h
) {
	if (!dst || !src) {
		gd_error_ex(GD_WARNING, "one image pointer passed to overflowCopyRectBounds is null, failing operation gracefully\n");
		return 1;
	}

	if (w <= 0 || h <= 0) {
		gd_error_ex(GD_WARNING, "one parameter to a memory allocation multiplication is negative or zero, failing operation gracefully\n");
		return 1;
	}
	if (w > 0 && dstX > INT_MAX - w) {
		gd_error_ex(GD_WARNING, "sum of destination x coordinate and width would overflow INT_MAX, failing operation gracefully\n");
		return 1;
	}

	if (h > 0 && dstY > INT_MAX - h) {
		gd_error_ex(GD_WARNING, "sum of destination y coordinate and height would overflow INT_MAX, failing operation gracefully\n");
		return 1;
	}

	if (w > 0 && srcX > INT_MAX - w) {
		gd_error_ex(GD_WARNING, "sum of destination y coordinate and height would overflow INT_MAX, failing operation gracefully\n");
		return 1;
	}

	if (h > 0 && srcY > INT_MAX - h) {
		gd_error_ex(GD_WARNING, "sum of source y coordinate and height would overflow INT_MAX, failing operation gracefully\n");
		return 1;
	}

	return 0;
}
