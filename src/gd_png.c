#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "png.h"

#define TRUE 1
#define FALSE 0

/* Exported functions */
extern void gdImagePng(gdImagePtr im, FILE *out);
extern gdImagePtr gdImageCreateFromPng(FILE *in);

gdImagePtr gdImageCreateFromPng(FILE *inFile)
{
	gdImagePtr im;
        gdIOCtx   *in = gdNewFileCtx(inFile);
        im = gdImageCreateFromPngCtx(in);
	in->free(in);
	return im;
}

void png_read_data(png_structp png_ptr, 
	png_bytep data, png_size_t length)
{
	gdGetBuf(data, length, (gdIOCtx *)
		png_get_io_ptr(png_ptr));	
}

gdImagePtr gdImageCreateFromPngCtx(gdIOCtxPtr in)
{
	gdImagePtr im;
	png_structp png_ptr;
	png_infop info_ptr;
	int i;
	png_ptr = malloc(sizeof(png_struct));
	if (!png_ptr) {
		return 0;
	}
	info_ptr = malloc(sizeof(png_info));
	if (!info_ptr) {
		return 0;
	}	
	if (setjmp(png_ptr->jmpbuf)) {
		png_write_destroy(png_ptr);
		free(png_ptr);
		free(info_ptr);
		return 0;
	}
	png_info_init(info_ptr);
	png_read_init(png_ptr);
	png_set_read_fn(png_ptr,
		(void *) in,
		png_read_data);
	/* Turn non-palette PNGs into palette PNGs. This will
		go away in a future 24-bit GD. It's not a very
		good way of doing things. */
	if (info_ptr->color_type & PNG_COLOR_MASK_COLOR) {
		png_colorp std_color_cube;
		int r, g, b;
		int i = 0;
		std_color_cube = malloc(sizeof(png_color) * 216);
		for (r = 0; (r < 6); r++) {
			for (g = 0; (g < 6); g++) {
				for (b = 0; (b < 6); b++) {
					std_color_cube[i].red =
						r * 51;
					std_color_cube[i].green =
						g * 51;
					std_color_cube[i].blue =
						b * 51;
					i++;
				}
			}
		}
		png_set_dither(png_ptr, std_color_cube,
			216, 216,
			NULL, 0);
	}
	png_read_info(png_ptr, info_ptr);
	if (info_ptr->valid &PNG_INFO_gAMA) {
		png_set_gamma(png_ptr, 2.0, info_ptr->gamma);
	} else {
		png_set_gamma(png_ptr, 2.0, 0.45);
	}
	im = gdImageCreate(info_ptr->width, 
		info_ptr->height);
	if (!im) {
		longjmp(png_ptr->jmpbuf, 1);		
	}
	im->interlace = info_ptr->interlace_type ? 1 : 0;
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	png_read_update_info(png_ptr, info_ptr);
	/* This works because gd's internal data structure
		happens to be compatible at this time. */
	png_read_image(png_ptr, (png_bytepp) im->pixels);	
	png_read_end(png_ptr, info_ptr);
	for (i = 0; (i < info_ptr -> num_palette); i++) {
		im->red[i] = info_ptr -> palette[i].red;
		im->green[i] = info_ptr -> palette[i].green;
		im->blue[i] = info_ptr -> palette[i].blue;
		im->open[i] = 0;
	}
	im->colorsTotal = info_ptr -> num_palette;
	png_destroy_read_struct(
		&png_ptr, &info_ptr,
		(png_infopp) 0);
	return im;
}

void png_write_data(png_structp png_ptr, 
	png_bytep data, png_size_t length)
{
	gdPutBuf(data, length, (gdIOCtx *)
		png_get_io_ptr(png_ptr));	
}

void png_flush_data(png_structp png_ptr)
{
}

static void _gdImagePng(gdImagePtr im, gdIOCtx *out)
{
	int x, y;
	int depth = 1;
	int allocated = 2;
	int i;
	png_structp png_ptr;
	png_infop info_ptr;
	png_ptr = malloc(sizeof(png_struct));
	if (!png_ptr) {
		return;
	}
	info_ptr = malloc(sizeof(png_info));
	if (!info_ptr) {
		return;
	}	
	if (setjmp(png_ptr->jmpbuf)) {
		png_write_destroy(png_ptr);
		free(png_ptr);
		free(info_ptr);
		return;
	}
	png_info_init(info_ptr);
	png_write_init(png_ptr);
	png_set_write_fn(png_ptr,
		(void *) out,
		png_write_data,
		png_flush_data);	
	while (allocated < im->colorsTotal) {
		allocated <<= 1;
		depth++;
	}
	/* Legal PNG palette bit depths are 1, 2, 4, and 8. */
	if (depth > 2) {
		if (depth > 4) {
			depth = 8;
		} else {
			depth = 4;
		}
	}
	info_ptr->width = im->sx;
	info_ptr->height = im->sy;
	info_ptr->bit_depth = depth;
	info_ptr->color_type = PNG_COLOR_TYPE_PALETTE;
	if (im->interlace) {
		info_ptr->interlace_type = 1;
	} else {
		info_ptr->interlace_type = 0;
	}
	info_ptr->valid |= PNG_INFO_PLTE;
	info_ptr->palette = (png_colorp) malloc(sizeof(png_color) *
		im->colorsTotal);
	for (i = 0; (i < im->colorsTotal); i++) {
		info_ptr->palette[i].red = im->red[i];
		info_ptr->palette[i].green = im->green[i];
		info_ptr->palette[i].blue = im->blue[i];
	}
	info_ptr->num_palette = im->colorsTotal;
	if (im->transparent != -1) {
		info_ptr->trans = (png_bytep) malloc(1);
		info_ptr->trans[0] = im->transparent;
		info_ptr->num_trans = 1;
	}	
	png_write_info(png_ptr, info_ptr);
	png_set_packing(png_ptr);
	/* NOTE: this is dependent on the current
		representation of pixels! It happens
		to be compatible with libpng. */
	png_write_image(png_ptr, (png_byte **) im->pixels);
	png_write_end(png_ptr, info_ptr);
	png_write_destroy(png_ptr);	
	if (info_ptr->palette) {
		free(info_ptr->palette);
	}
	if (info_ptr->trans) {
		free(info_ptr->trans);
	}
	free(png_ptr);
	free(info_ptr);
}

void gdImagePng(gdImagePtr im, FILE *outFile)
{
        gdIOCtx   *out = gdNewFileCtx(outFile);
        _gdImagePng(im, out);
	out->free(out);
}

void gdImagePngCtx(gdImagePtr im, gdIOCtx *out)
{
        _gdImagePng(im, out);
}

void* gdImagePngPtr(gdImagePtr im, int *size)
{
	void	*rv;
        gdIOCtx   *out = gdNewDynamicCtx(2048, NULL);
        _gdImagePng(im, out);
        rv = gdDPExtractData(out, size);
	out->free(out);
	return rv;
}


