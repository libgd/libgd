/*
	gd_bmp.c

	Bitmap format support for libgd

	* Written 2007, Scott MacVicar
	---------------------------------------------------------------------------

	Todo:

	Bitfield encoding

	----------------------------------------------------------------------------
 */

/**
 * File: BMP IO
 *
 * Read and write BMP images.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "gdhelpers.h"
#include "gd_errors.h"
#include "bmp.h"

static int compress_row(unsigned char *uncompressed_row, int length);
static int build_rle_packet(unsigned char *row, int packet_type, int length, unsigned char *data);

static int bmp_read_header(gdIOCtxPtr infile, bmp_hdr_t *hdr);
static int bmp_read_info(gdIOCtxPtr infile, bmp_info_t *info);
static int bmp_read_windows_v3_info(gdIOCtxPtr infile, bmp_info_t *info);
static int bmp_read_os2_v1_info(gdIOCtxPtr infile, bmp_info_t *info);
static int bmp_read_os2_v2_info(gdIOCtxPtr infile, bmp_info_t *info);

static int bmp_read_direct(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header);
static int bmp_read_1bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header);
static int bmp_read_2bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header);
static int bmp_read_4bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header);
static int bmp_read_8bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header);
static int bmp_read_rle(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info);

static int _gdImageBmpCtx(gdImagePtr im, gdIOCtxPtr out, int compression);

static int bmp_validate_info(bmp_info_t *info, bmp_hdr_t *hdr);
static int bmp_read_bitfield_masks(gdIOCtxPtr infile, bmp_info_t *info, int read_alpha);
static int bmp_skip_bytes(gdIOCtxPtr infile, int count);
static int bmp_check_palette_index(gdImagePtr im, int index);
static int bmp_row_padding(int width, int depth, int *padding);
static int bmp_image_size(int width, int height, int depth, int *size);
static int bmp_get_mask_shift(unsigned int mask);
static int bmp_get_mask_bits(unsigned int mask);
static int bmp_extract_mask(unsigned int pixel, unsigned int mask);
static int bmp_alpha_to_gd(int alpha);

#define BMP_DEBUG(s)

static int gdBMPPutWord(gdIOCtx *out, int w)
{
	/* Byte order is little-endian */
	gdPutC(w & 0xFF, out);
	gdPutC((w >> 8) & 0xFF, out);
	return 0;
}

static int gdBMPPutInt(gdIOCtx *out, int w)
{
	/* Byte order is little-endian */
	gdPutC(w & 0xFF, out);
	gdPutC((w >> 8) & 0xFF, out);
	gdPutC((w >> 16) & 0xFF, out);
	gdPutC((w >> 24) & 0xFF, out);
	return 0;
}

/*
	Function: gdImageBmpPtr

	Outputs the given image as BMP data, but using a <gdIOCtx> instead
	of a file. See <gdImageBmp>.

	Parameters:
		im			- the image to save.
		size 		- Output: size in bytes of the result.
		compression - whether to apply RLE or not.

	Returns:

		A pointer to memory containing the image data or NULL on error.
*/
BGD_DECLARE(void *) gdImageBmpPtr(gdImagePtr im, int *size, int compression)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) return NULL;
	if (!_gdImageBmpCtx(im, out, compression))
		rv = gdDPExtractData(out, size);
	else
		rv = NULL;
	out->gd_free(out);
	return rv;
}

/*
	Function: gdImageBmp

    <gdImageBmp> outputs the specified image to the specified file in
    BMP format. The file must be open for writing. Under MSDOS and all
    versions of Windows, it is important to use "wb" as opposed to
    simply "w" as the mode when opening the file, and under Unix there
    is no penalty for doing so. <gdImageBmp> does not close the file;
    your code must do so.

    In addition, <gdImageBmp> allows to specify whether RLE compression
    should be applied.

	Variants:

		<gdImageBmpCtx> write via a <gdIOCtx> instead of a file handle.

		<gdImageBmpPtr> store the image file to memory.

	Parameters:

		im			- the image to save.
		outFile		- the output FILE* object.
		compression - whether to apply RLE or not.

	Returns:
		nothing
*/
BGD_DECLARE(void) gdImageBmp(gdImagePtr im, FILE *outFile, int compression)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) return;
	gdImageBmpCtx(im, out, compression);
	out->gd_free(out);
}

/*
	Function: gdImageBmpCtx

	Outputs the given image as BMP data, but using a <gdIOCtx> instead
	of a file. See <gdImageBmp>.

	Parameters:
		im			- the image to save.
		out 		- the <gdIOCtx> to write to.
		compression - whether to apply RLE or not.
*/
BGD_DECLARE(void) gdImageBmpCtx(gdImagePtr im, gdIOCtxPtr out, int compression)
{
	_gdImageBmpCtx(im, out, compression);
}

static int _gdImageBmpCtx(gdImagePtr im, gdIOCtxPtr out, int compression)
{
	int bitmap_size = 0, info_size, total_size, padding;
	int i, row, xpos, pixel;
	int error = 0;
	unsigned char *uncompressed_row = NULL, *uncompressed_row_start = NULL;
	FILE *tmpfile_for_compression = NULL;
	gdIOCtxPtr out_original = NULL;
	int ret = 1;

	/* No compression if its true colour or we don't support seek */
	if (im->trueColor) {
		compression = 0;
	}

	if (compression && !out->seek) {
		/* Try to create a temp file where we can seek */
		if ((tmpfile_for_compression = tmpfile()) == NULL) {
			compression = 0;
		} else {
			out_original = out;
			if ((out = (gdIOCtxPtr)gdNewFileCtx(tmpfile_for_compression)) == NULL) {
				out = out_original;
				out_original = NULL;
				compression = 0;
			}
		}
	}

	bitmap_size = ((im->sx * (im->trueColor ? 24 : 8)) / 8) * im->sy;

	/* 40 byte Windows v3 header */
	info_size = BMP_WINDOWS_V3;

	/* data for the palette */
	if (!im->trueColor) {
		info_size += im->colorsTotal * 4;
		if (compression) {
			bitmap_size = 0;
		}
	}

	/* The line must be divisible by 4, else its padded with NULLs */
	padding = ((int)(im->trueColor ? 3 : 1) * im->sx) % 4;
	if (padding) {
		padding = 4 - padding;
	}

	/* bitmap header + info header + data */
	total_size = 14 + info_size + bitmap_size;

	/* write bmp header info */
	gdPutBuf("BM", 2, out);
	gdBMPPutInt(out, total_size + padding * im->sy);
	gdBMPPutWord(out, 0);
	gdBMPPutWord(out, 0);
	gdBMPPutInt(out, 14 + info_size);

	/* write Windows v3 headers */
	gdBMPPutInt(out, BMP_WINDOWS_V3); /* header size */
	gdBMPPutInt(out, im->sx); /* width */
	gdBMPPutInt(out, im->sy); /* height */
	gdBMPPutWord(out, 1); /* colour planes */
	gdBMPPutWord(out, (im->trueColor ? 24 : 8)); /* bit count */
	gdBMPPutInt(out, (compression ? BMP_BI_RLE8 : BMP_BI_RGB)); /* compression */
	gdBMPPutInt(out, bitmap_size + padding * im->sy); /* image size */
	gdBMPPutInt(out, 0); /* H resolution */
	gdBMPPutInt(out, 0); /* V ressolution */
	gdBMPPutInt(out, im->colorsTotal); /* colours used */
	gdBMPPutInt(out, 0); /* important colours */

	/* 8-bit colours */
	if (!im->trueColor) {
		for(i = 0; i< im->colorsTotal; ++i) {
			gdPutC(gdImageBlue(im, i), out);
			gdPutC(gdImageGreen(im, i), out);
			gdPutC(gdImageRed(im, i), out);
			gdPutC(0, out);
		}

		if (compression) {
			/* Can potentially change this to X + ((X / 128) * 3) */
			uncompressed_row = uncompressed_row_start = (unsigned char *) gdCalloc(gdImageSX(im) * 2, sizeof(char));
			if (!uncompressed_row) {
				/* malloc failed */
				goto cleanup;
			}
		}

		for (row = (im->sy - 1); row >= 0; row--) {
			if (compression) {
				memset (uncompressed_row, 0, gdImageSX(im));
			}

			for (xpos = 0; xpos < im->sx; xpos++) {
				if (compression) {
					*uncompressed_row++ = (unsigned char)gdImageGetPixel(im, xpos, row);
				} else {
					gdPutC(gdImageGetPixel(im, xpos, row), out);
				}
			}

			if (!compression) {
				/* Add padding to make sure we have n mod 4 == 0 bytes per row */
				for (xpos = padding; xpos > 0; --xpos) {
					gdPutC('\0', out);
				}
			} else {
				int compressed_size = 0;
				uncompressed_row = uncompressed_row_start;
				if ((compressed_size = compress_row(uncompressed_row, gdImageSX(im))) < 0) {
					error = 1;
					break;
				}
				bitmap_size += compressed_size;


				if (gdPutBuf(uncompressed_row, compressed_size, out) != compressed_size){
					gd_error("gd-bmp write error\n");
					error = 1;
					break;
				}
				gdPutC(BMP_RLE_COMMAND, out);
				gdPutC(BMP_RLE_ENDOFLINE, out);
				bitmap_size += 2;
			}
		}

		if (compression && uncompressed_row) {
			gdFree(uncompressed_row);
			if (error != 0) {
				goto cleanup;
			}
			/* Update filesize based on new values and set compression flag */
			gdPutC(BMP_RLE_COMMAND, out);
			gdPutC(BMP_RLE_ENDOFBITMAP, out);
			bitmap_size += 2;

			/* Write new total bitmap size */
			gdSeek(out, 2);
			gdBMPPutInt(out, total_size + bitmap_size);

			/* Total length of image data */
			gdSeek(out, 34);
			gdBMPPutInt(out, bitmap_size);
		}

	} else {
		for (row = (im->sy - 1); row >= 0; row--) {
			for (xpos = 0; xpos < im->sx; xpos++) {
				pixel = gdImageGetPixel(im, xpos, row);

				gdPutC(gdTrueColorGetBlue(pixel), out);
				gdPutC(gdTrueColorGetGreen(pixel), out);
				gdPutC(gdTrueColorGetRed(pixel), out);
			}

			/* Add padding to make sure we have n mod 4 == 0 bytes per row */
			for (xpos = padding; xpos > 0; --xpos) {
				gdPutC('\0', out);
			}
		}
	}


	/* If we needed a tmpfile for compression copy it over to out_original */
	if (tmpfile_for_compression) {
		unsigned char* copy_buffer = NULL;
		int buffer_size = 0;

		gdSeek(out, 0);
		copy_buffer = (unsigned char *) gdMalloc(1024 * sizeof(unsigned char));
		if (copy_buffer == NULL) {
			goto cleanup;
		}

		while ((buffer_size = gdGetBuf(copy_buffer, 1024, out)) != EOF) {
			if (buffer_size == 0) {
				break;
			}
			if (gdPutBuf(copy_buffer , buffer_size, out_original) != buffer_size) {
				gd_error("gd-bmp write error\n");
				error = 1;
			}
		}
		gdFree(copy_buffer);

		/* Replace the temp with the original which now has data */
		out->gd_free(out);
		out = out_original;
		out_original = NULL;
	}

	ret = error;
cleanup:
	if (tmpfile_for_compression) {
#ifdef _WIN32
		_rmtmp();
#else
		fclose(tmpfile_for_compression);
#endif
		tmpfile_for_compression = NULL;
	}

	if (out_original) {
		out_original->gd_free(out_original);
	}
	return ret;
}

static int compress_row(unsigned char *row, int length)
{
	int rle_type = 0;
	int compressed_length = 0;
	int pixel = 0, compressed_run = 0, rle_compression = 0;
	unsigned char *uncompressed_row = NULL, *uncompressed_rowp = NULL, *uncompressed_start = NULL;

	uncompressed_row = (unsigned char *) gdMalloc(length);
	if (!uncompressed_row) {
		return -1;
	}

	memcpy(uncompressed_row, row, length);
	uncompressed_start = uncompressed_rowp = uncompressed_row;

	for (pixel = 0; pixel < length; pixel++) {
		if (compressed_run == 0) {
			uncompressed_row = uncompressed_rowp;
			compressed_run++;
			uncompressed_rowp++;
			rle_type = BMP_RLE_TYPE_RAW;
			continue;
		}

		if (compressed_run == 1) {
			/* Compare next byte */
			if (memcmp(uncompressed_rowp, uncompressed_rowp - 1, 1) == 0) {
				rle_type = BMP_RLE_TYPE_RLE;
			}
		}

		if (rle_type == BMP_RLE_TYPE_RLE) {
			if (compressed_run >= 128 || memcmp(uncompressed_rowp, uncompressed_rowp - 1, 1) != 0) {
				/* more than what we can store in a single run or run is over due to non match, force write */
				rle_compression = build_rle_packet(row, rle_type, compressed_run, uncompressed_row);
				row += rle_compression;
				compressed_length += rle_compression;
				compressed_run = 0;
				pixel--;
			} else {
				compressed_run++;
				uncompressed_rowp++;
			}
		} else {
			if (compressed_run >= 128 || memcmp(uncompressed_rowp, uncompressed_rowp - 1, 1) == 0) {
				/* more than what we can store in a single run or run is over due to match, force write */
				rle_compression = build_rle_packet(row, rle_type, compressed_run, uncompressed_row);
				row += rle_compression;
				compressed_length += rle_compression;
				compressed_run = 0;
				pixel--;
			} else {
				/* add this pixel to the row */
				compressed_run++;
				uncompressed_rowp++;
			}

		}
	}

	if (compressed_run) {
		compressed_length += build_rle_packet(row, rle_type, compressed_run, uncompressed_row);
	}

	gdFree(uncompressed_start);

	return compressed_length;
}

static int build_rle_packet(unsigned char *row, int packet_type, int length, unsigned char *data)
{
	int compressed_size = 0;
	if (length < 1 || length > 128) {
		return 0;
	}

	/* Bitmap specific cases is that we can't have uncompressed rows of length 1 or 2 */
	if (packet_type == BMP_RLE_TYPE_RAW && length < 3) {
		int i = 0;
		for (i = 0; i < length; i++) {
			compressed_size += 2;
			memset(row, 1, 1);
			row++;

			memcpy(row, data++, 1);
			row++;
		}
	} else if (packet_type == BMP_RLE_TYPE_RLE) {
		compressed_size = 2;
		memset(row, length, 1);
		row++;

		memcpy(row, data, 1);
		row++;
	} else {
		compressed_size = 2 + length;
		memset(row, BMP_RLE_COMMAND, 1);
		row++;

		memset(row, length, 1);
		row++;

		memcpy(row, data, length);
		row += length;

		/* Must be an even number for an uncompressed run */
		if (length % 2) {
			memset(row, 0, 1);
			row++;
			compressed_size++;
		}
	}
	return compressed_size;
}

/*
	Function: gdImageCreateFromBmp
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromBmp(FILE * inFile)
{
	gdImagePtr im = 0;
	gdIOCtx *in = gdNewFileCtx(inFile);
	if (in == NULL) return NULL;
	im = gdImageCreateFromBmpCtx(in);
	in->gd_free(in);
	return im;
}

/*
	Function: gdImageCreateFromBmpPtr
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromBmpPtr(int size, void *data)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	if (in == NULL) return NULL;
	im = gdImageCreateFromBmpCtx(in);
	in->gd_free(in);
	return im;
}

/*
	Function: gdImageCreateFromBmpCtx
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromBmpCtx(gdIOCtxPtr infile)
{
	bmp_hdr_t *hdr;
	bmp_info_t *info;
	gdImagePtr im = NULL;
	int error = 0;

	if (!(hdr= (bmp_hdr_t *)gdCalloc(1, sizeof(bmp_hdr_t)))) {
		return NULL;
	}

	if (bmp_read_header(infile, hdr)) {
		gdFree(hdr);
		return NULL;
	}

	if (hdr->magic != 0x4d42) {
		gdFree(hdr);
		return NULL;
	}

	if (!(info = (bmp_info_t *)gdCalloc(1, sizeof(bmp_info_t)))) {
		gdFree(hdr);
		return NULL;
	}

	if (bmp_read_info(infile, info)) {
		gdFree(hdr);
		gdFree(info);
		return NULL;
	}

	if (bmp_validate_info(info, hdr)) {
		gdFree(hdr);
		gdFree(info);
		return NULL;
	}

	BMP_DEBUG(printf("Numcolours: %d\n", info->numcolors));
	BMP_DEBUG(printf("Width: %d\n", info->width));
	BMP_DEBUG(printf("Height: %d\n", info->height));
	BMP_DEBUG(printf("Planes: %d\n", info->numplanes));
	BMP_DEBUG(printf("Depth: %d\n", info->depth));
	BMP_DEBUG(printf("Offset: %d\n", hdr->off));

	if (info->depth >= 16) {
		im = gdImageCreateTrueColor(info->width, info->height);
	} else {
		im = gdImageCreate(info->width, info->height);
	}

	if (!im) {
		gdFree(hdr);
		gdFree(info);
		return NULL;
	}

	switch (info->depth) {
	case 1:
		BMP_DEBUG(printf("1-bit image\n"));
		error = bmp_read_1bit(im, infile, info, hdr);
		break;
	case 2:
		BMP_DEBUG(printf("2-bit image\n"));
		error = bmp_read_2bit(im, infile, info, hdr);
		break;
	case 4:
		BMP_DEBUG(printf("4-bit image\n"));
		error = bmp_read_4bit(im, infile, info, hdr);
		break;
	case 8:
		BMP_DEBUG(printf("8-bit image\n"));
		error = bmp_read_8bit(im, infile, info, hdr);
		break;
	case 16:
	case 24:
	case 32:
		BMP_DEBUG(printf("Direct BMP image\n"));
		error = bmp_read_direct(im, infile, info, hdr);
		break;
	default:
		BMP_DEBUG(printf("Unknown bit count\n"));
		error = 1;
	}

	gdFree(hdr);
	gdFree(info);

	if (error) {
		gdImageDestroy(im);
		return NULL;
	}

	return im;
}

static int bmp_read_header(gdIOCtx *infile, bmp_hdr_t *hdr)
{
	if(
	    !gdGetWordLSB(&hdr->magic, infile) ||
	    !gdGetIntLSB(&hdr->size, infile) ||
	    !gdGetWordLSB(&hdr->reserved1, infile) ||
	    !gdGetWordLSB(&hdr->reserved2 , infile) ||
	    !gdGetIntLSB(&hdr->off , infile)
	) {
		return 1;
	}
	return 0;
}

static int bmp_read_info(gdIOCtx *infile, bmp_info_t *info)
{
	/* read BMP length so we can work out the version */
	if (!gdGetIntLSB(&info->len, infile)) {
		return 1;
	}

	switch (info->len) {
		/* For now treat Windows v4 + v5 as v3 */
	case BMP_WINDOWS_V3:
	case BMP_WINDOWS_V2:
	case BMP_WINDOWS_V3_ALPHA:
	case BMP_WINDOWS_V4:
	case BMP_WINDOWS_V5:
		BMP_DEBUG(printf("Reading Windows Header\n"));
		if (bmp_read_windows_v3_info(infile, info)) {
			return 1;
		}
		break;
	case BMP_OS2_V1:
		if (bmp_read_os2_v1_info(infile, info)) {
			return 1;
		}
		break;
	case BMP_OS2_V2_SHORT:
	case BMP_OS2_V2:
		if (bmp_read_os2_v2_info(infile, info)) {
			return 1;
		}
		break;
	default:
		BMP_DEBUG(printf("Unhandled bitmap\n"));
		return 1;
	}
	return 0;
}

static int bmp_skip_bytes(gdIOCtxPtr infile, int count)
{
	int i, c;

	for (i = 0; i < count; i++) {
		if (!gdGetByte(&c, infile)) {
			return 1;
		}
	}
	return 0;
}

static int bmp_read_bitfield_masks(gdIOCtxPtr infile, bmp_info_t *info, int read_alpha)
{
	int red, green, blue, alpha;

	if (
	    !gdGetIntLSB(&red, infile) ||
	    !gdGetIntLSB(&green, infile) ||
	    !gdGetIntLSB(&blue, infile)
	) {
		return 1;
	}

	info->red_mask = (unsigned int) red;
	info->green_mask = (unsigned int) green;
	info->blue_mask = (unsigned int) blue;

	if (read_alpha) {
		if (!gdGetIntLSB(&alpha, infile)) {
			return 1;
		}
		info->alpha_mask = (unsigned int) alpha;
	}

	return 0;
}

static int bmp_validate_info(bmp_info_t *info, bmp_hdr_t *hdr)
{
	int image_size;
	int min_size;

	if (hdr->off < 14 + info->len || hdr->size < 0 || hdr->off < 0) {
		return 1;
	}

	if (info->numplanes != 1) {
		return 1;
	}

	if (info->depth != 1 && info->depth != 2 && info->depth != 4 && info->depth != 8 &&
	        info->depth != 16 && info->depth != 24 && info->depth != 32) {
		return 1;
	}

	if (info->topdown &&
	        (info->enctype == BMP_BI_RLE4 || info->enctype == BMP_BI_RLE8)) {
		return 1;
	}

	if (overflow2(info->width, info->height)) {
		return 1;
	}

	if (info->hres < 0 || info->vres < 0) {
		return 1;
	}
	if (info->hres > 0 && info->vres > 0 &&
	        (info->hres / info->vres > 1000 || info->vres / info->hres > 1000)) {
		return 1;
	}

	if (info->enctype == BMP_BI_RGB || info->enctype == BMP_BI_BITFIELDS ||
	        info->enctype == BMP_BI_ALPHABITFIELDS) {
		if (bmp_image_size(info->width, info->height, info->depth, &image_size)) {
			return 1;
		}
		if (info->size != 0 && info->size != image_size) {
			return 1;
		}
		if (hdr->size != 0) {
			min_size = hdr->off + image_size;
			if (min_size < hdr->off) {
				return 1;
			}
			if (hdr->size < min_size && hdr->size != 14 + info->len) {
				return 1;
			}
			if (hdr->size > min_size && hdr->size - min_size > 1024 * 1024) {
				return 1;
			}
		}
	}

	return 0;
}

static int bmp_read_windows_v3_info(gdIOCtxPtr infile, bmp_info_t *info)
{
	int extra = info->len - BMP_WINDOWS_V3;

	if (
	    !gdGetIntLSB(&info->width, infile) ||
	    !gdGetIntLSB(&info->height, infile) ||
	    !gdGetWordLSB(&info->numplanes, infile) ||
	    !gdGetWordLSB(&info->depth, infile) ||
	    !gdGetIntLSB(&info->enctype, infile) ||
	    !gdGetIntLSB(&info->size, infile) ||
	    !gdGetIntLSB(&info->hres, infile) ||
	    !gdGetIntLSB(&info->vres, infile) ||
	    !gdGetIntLSB(&info->numcolors, infile) ||
	    !gdGetIntLSB(&info->mincolors, infile)
	) {
		return 1;
	}

	/* Unlikely, but possible -- largest signed value won't fit in unsigned. */
	if (info->height == 0 || info->height == INT_MIN)
		return 1;
	if (info->height < 0) {
		info->topdown = 1;
		info->height = -info->height;
	} else {
		info->topdown = 0;
	}

	info->type = BMP_PALETTE_4;

	if (extra > 0) {
		if (extra < 12 || bmp_read_bitfield_masks(infile, info, extra >= 16)) {
			return 1;
		}
		extra -= (extra >= 16) ? 16 : 12;
		if (bmp_skip_bytes(infile, extra)) {
			return 1;
		}
	}

	/* Height was checked above. */
	if (info->width <= 0 || info->numplanes <= 0 || info->depth <= 0 ||
	        info->numcolors < 0 || info->mincolors < 0) {
		return 1;
	}

	return 0;
}

static int bmp_read_os2_v1_info(gdIOCtxPtr infile, bmp_info_t *info)
{
	if (
	    !gdGetWordLSB((signed short int *)&info->width, infile) ||
	    !gdGetWordLSB((signed short int *)&info->height, infile) ||
	    !gdGetWordLSB(&info->numplanes, infile) ||
	    !gdGetWordLSB(&info->depth, infile)
	) {
		return 1;
	}

	/* OS2 v1 doesn't support topdown */
	info->topdown = 0;

	/* The spec says the depth can only be a few value values. */
	if (info->depth != 1 && info->depth != 4 && info->depth != 8 &&
	        info->depth != 16 && info->depth != 24) {
		return 1;
	}

	info->numcolors = (info->depth <= 8) ? (1 << info->depth) : 0;
	info->type = BMP_PALETTE_3;

	if (info->width <= 0 || info->height <= 0 || info->numplanes <= 0) {
		return 1;
	}

	return 0;
}

static int bmp_read_os2_v2_info(gdIOCtxPtr infile, bmp_info_t *info)
{
	char useless_bytes[24];
	if (
	    !gdGetIntLSB(&info->width, infile) ||
	    !gdGetIntLSB(&info->height, infile) ||
	    !gdGetWordLSB(&info->numplanes, infile) ||
	    !gdGetWordLSB(&info->depth, infile)
	) {
		return 1;
	}

	if (info->len == BMP_OS2_V2_SHORT) {
		info->enctype = BMP_BI_RGB;
		info->size = 0;
		info->hres = 0;
		info->vres = 0;
		info->numcolors = (info->depth <= 8) ? (1 << info->depth) : 0;
		info->mincolors = 0;
		goto done;
	}

	if (
	    !gdGetIntLSB(&info->enctype, infile) ||
	    !gdGetIntLSB(&info->size, infile) ||
	    !gdGetIntLSB(&info->hres, infile) ||
	    !gdGetIntLSB(&info->vres, infile) ||
	    !gdGetIntLSB(&info->numcolors, infile) ||
	    !gdGetIntLSB(&info->mincolors, infile)
	) {
		return 1;
	}

	/* Lets seek the next 24 pointless bytes, we don't care too much about it */
	if (!gdGetBuf(useless_bytes, 24, infile)) {
		return 1;
	}

done:
	/* Unlikely, but possible -- largest signed value won't fit in unsigned. */
	if (info->height == 0 || info->height == INT_MIN)
		return 1;
	if (info->height < 0) {
		info->topdown = 1;
		info->height = -info->height;
	} else {
		info->topdown = 0;
	}

	info->type = BMP_PALETTE_4;

	/* Height was checked above. */
	if (info->width <= 0 || info->numplanes <= 0 || info->depth <= 0 ||
	        info->numcolors < 0 || info->mincolors < 0) {
		return 1;
	}

	return 0;
}

static int bmp_read_direct(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header)
{
	int ypos = 0, xpos = 0, row = 0;
	int padding = 0, red = 0, green = 0, blue = 0;
	int alpha = gdAlphaOpaque;
	signed short int data16 = 0;
	int data32 = 0;
	unsigned int mask = 0;

	if (info->depth == 16 && info->enctype == BMP_BI_RGB) {
		info->red_mask = 0x7C00;
		info->green_mask = 0x03E0;
		info->blue_mask = 0x001F;
	} else if (info->depth == 32 && info->enctype == BMP_BI_RGB) {
		info->red_mask = 0x00FF0000;
		info->green_mask = 0x0000FF00;
		info->blue_mask = 0x000000FF;
	}

	switch(info->enctype) {
	case BMP_BI_RGB:
		/* no-op */
		break;

	case BMP_BI_BITFIELDS:
		if (info->depth == 24) {
			BMP_DEBUG(printf("Bitfield compression isn't supported for 24-bit\n"));
			return 1;
		}
		if (info->len == BMP_WINDOWS_V3 && bmp_read_bitfield_masks(infile, info, 0)) {
			return 1;
		}
		break;
	case BMP_BI_ALPHABITFIELDS:
		if (info->depth != 16 && info->depth != 32) {
			return 1;
		}
		if (info->len == BMP_WINDOWS_V3 && bmp_read_bitfield_masks(infile, info, 1)) {
			return 1;
		}
		break;

	case BMP_BI_RLE8:
		if (info->depth != 8) {
			BMP_DEBUG(printf("RLE is only valid for 8-bit images\n"));
			return 1;
		}
		break;
	case BMP_BI_RLE4:
		if (info->depth != 4) {
			BMP_DEBUG(printf("RLE is only valid for 4-bit images\n"));
			return 1;
		}
		break;
	case BMP_BI_JPEG:
	case BMP_BI_PNG:
	default:
		BMP_DEBUG(printf("Unsupported BMP compression format\n"));
		return 1;
	}

	if (info->depth == 16 || info->depth == 32) {
		mask = info->red_mask | info->green_mask | info->blue_mask;
		if (info->red_mask == 0 || info->green_mask == 0 || info->blue_mask == 0) {
			return 1;
		}
		if ((info->red_mask & info->green_mask) || (info->red_mask & info->blue_mask) ||
		        (info->green_mask & info->blue_mask)) {
			return 1;
		}
		if (info->depth == 16 && (mask & 0xFFFF0000U)) {
			return 1;
		}
	}

	/* There is a chance the data isn't until later, would be weird but it is possible */
	if (gdTell(infile) != header->off) {
		/* Should make sure we don't seek past the file size */
		if (!gdSeek(infile, header->off)) {
			return 1;
		}
	}

	/* The line must be divisible by 4, else its padded with NULLs */
	if (bmp_row_padding(info->width, info->depth, &padding)) {
		return 1;
	}


	for (ypos = 0; ypos < info->height; ++ypos) {
		if (info->topdown) {
			row = ypos;
		} else {
			row = info->height - ypos - 1;
		}

		for (xpos = 0; xpos < info->width; xpos++) {
			if (info->depth == 16) {
				if (!gdGetWordLSB(&data16, infile)) {
					return 1;
				}
				BMP_DEBUG(printf("Data: %X\n", data16));
				red = bmp_extract_mask((unsigned short) data16, info->red_mask);
				green = bmp_extract_mask((unsigned short) data16, info->green_mask);
				blue = bmp_extract_mask((unsigned short) data16, info->blue_mask);
				alpha = info->alpha_mask ? bmp_alpha_to_gd(bmp_extract_mask((unsigned short) data16, info->alpha_mask)) : gdAlphaOpaque;
				BMP_DEBUG(printf("R: %d, G: %d, B: %d\n", red, green, blue));
			} else if (info->depth == 24) {
				if (!gdGetByte(&blue, infile) || !gdGetByte(&green, infile) || !gdGetByte(&red, infile)) {
					return 1;
				}
				alpha = gdAlphaOpaque;
			} else {
				if (!gdGetIntLSB(&data32, infile)) {
					return 1;
				}
				red = bmp_extract_mask((unsigned int) data32, info->red_mask);
				green = bmp_extract_mask((unsigned int) data32, info->green_mask);
				blue = bmp_extract_mask((unsigned int) data32, info->blue_mask);
				alpha = info->alpha_mask ? bmp_alpha_to_gd(bmp_extract_mask((unsigned int) data32, info->alpha_mask)) : gdAlphaOpaque;
			}
			gdImageSetPixel(im, xpos, row, gdTrueColorAlpha(red, green, blue, alpha));
		}
		for (xpos = padding; xpos > 0; --xpos) {
			if (!gdGetByte(&red, infile)) {
				return 1;
			}
		}
	}

	return 0;
}

static int bmp_read_palette(gdImagePtr im, gdIOCtxPtr infile, int count, int read_four)
{
	int i;
	int r, g, b, z;

	for (i = 0; i < count; i++) {
		if (
		    !gdGetByte(&b, infile) ||
		    !gdGetByte(&g, infile) ||
		    !gdGetByte(&r, infile) ||
		    (read_four && !gdGetByte(&z, infile))
		) {
			return 1;
		}
		im->red[i] = r;
		im->green[i] = g;
		im->blue[i] = b;
		im->open[i] = 1;
	}
	return 0;
}

static int bmp_check_palette_index(gdImagePtr im, int index)
{
	return index >= 0 && index < im->colorsTotal;
}

static int bmp_row_padding(int width, int depth, int *padding)
{
	int bits_per_row;
	int bytes_per_row;

	if (overflow2(width, depth)) {
		return 1;
	}
	bits_per_row = width * depth;
	if (bits_per_row > INT_MAX - 31) {
		return 1;
	}
	bytes_per_row = ((bits_per_row + 31) / 32) * 4;
	*padding = bytes_per_row - ((bits_per_row + 7) / 8);
	return 0;
}

static int bmp_image_size(int width, int height, int depth, int *size)
{
	int bits_per_row;
	int bytes_per_row;

	if (overflow2(width, depth)) {
		return 1;
	}
	bits_per_row = width * depth;
	if (bits_per_row > INT_MAX - 31) {
		return 1;
	}
	bytes_per_row = ((bits_per_row + 31) / 32) * 4;
	if (overflow2(bytes_per_row, height)) {
		return 1;
	}
	*size = bytes_per_row * height;
	return 0;
}

static int bmp_get_mask_shift(unsigned int mask)
{
	int shift = 0;

	if (mask == 0) {
		return 0;
	}
	while ((mask & 1U) == 0) {
		mask >>= 1;
		shift++;
	}
	return shift;
}

static int bmp_get_mask_bits(unsigned int mask)
{
	int bits = 0;

	if (mask == 0) {
		return 0;
	}
	mask >>= bmp_get_mask_shift(mask);
	while (mask & 1U) {
		bits++;
		mask >>= 1;
	}
	return bits;
}

static int bmp_extract_mask(unsigned int pixel, unsigned int mask)
{
	unsigned int value;
	int bits;

	if (mask == 0) {
		return 0;
	}
	value = (pixel & mask) >> bmp_get_mask_shift(mask);
	bits = bmp_get_mask_bits(mask);
	if (bits <= 0) {
		return 0;
	}
	if (bits >= 32) {
		return (int) (value >> 24);
	}
	if (bits >= 8) {
		return (int) ((value * 255U) / ((1U << bits) - 1U));
	}
	return (int) ((value * 255U + ((1U << bits) - 1U) / 2U) / ((1U << bits) - 1U));
}

static int bmp_alpha_to_gd(int alpha)
{
	if (alpha <= 0) {
		return gdAlphaTransparent;
	}
	if (alpha >= 255) {
		return gdAlphaOpaque;
	}
	return gdAlphaMax - (alpha * gdAlphaMax + 127) / 255;
}

static int bmp_read_1bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header)
{
	int ypos = 0, xpos = 0, row = 0, index = 0;
	int padding = 0, current_byte = 0, bit = 0;

	if (info->enctype != BMP_BI_RGB) {
		return 1;
	}

	if (!info->numcolors) {
		info->numcolors = 2;
	} else if (info->numcolors < 0 || info->numcolors > 2) {
		return 1;
	}

	if (bmp_read_palette(im, infile, info->numcolors, (info->type == BMP_PALETTE_4))) {
		return 1;
	}

	im->colorsTotal = info->numcolors;

	/* There is a chance the data isn't until later, would be weird but it is possible */
	if (gdTell(infile) != header->off) {
		/* Should make sure we don't seek past the file size */
		if (!gdSeek(infile, header->off)) {
			return 1;
		}
	}

	if (bmp_row_padding(info->width, info->depth, &padding)) {
		return 1;
	}

	for (ypos = 0; ypos < info->height; ++ypos) {
		if (info->topdown) {
			row = ypos;
		} else {
			row = info->height - ypos - 1;
		}

		for (xpos = 0; xpos < info->width; xpos += 8) {
			/* Bitmaps are always aligned in bytes so we'll never overflow */
			if (!gdGetByte(&current_byte, infile)) {
				return 1;
			}

			for (bit = 0; bit < 8; bit++) {
				index = ((current_byte & (0x80 >> bit)) != 0 ? 0x01 : 0x00);
				/* No need to read anything extra */
				if ((xpos + bit) >= info->width) {
					break;
				}
				if (!bmp_check_palette_index(im, index)) {
					return 1;
				}
				if (im->open[index]) {
					im->open[index] = 0;
				}
				gdImageSetPixel(im, xpos + bit, row, index);
			}
		}

		for (xpos = padding; xpos > 0; --xpos) {
			if (!gdGetByte(&index, infile)) {
				return 1;
			}
		}
	}
	return 0;
}

static int bmp_read_2bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header)
{
	int ypos = 0, xpos = 0, row = 0, index = 0;
	int padding = 0, current_byte = 0, shift = 0;

	if (info->enctype != BMP_BI_RGB) {
		return 1;
	}

	if (!info->numcolors) {
		info->numcolors = 4;
	} else if (info->numcolors < 0 || info->numcolors > 4) {
		return 1;
	}

	if (bmp_read_palette(im, infile, info->numcolors, (info->type == BMP_PALETTE_4))) {
		return 1;
	}

	im->colorsTotal = info->numcolors;

	if (gdTell(infile) != header->off) {
		if (!gdSeek(infile, header->off)) {
			return 1;
		}
	}

	if (bmp_row_padding(info->width, info->depth, &padding)) {
		return 1;
	}

	for (ypos = 0; ypos < info->height; ++ypos) {
		if (info->topdown) {
			row = ypos;
		} else {
			row = info->height - ypos - 1;
		}

		for (xpos = 0; xpos < info->width; xpos += 4) {
			if (!gdGetByte(&current_byte, infile)) {
				return 1;
			}

			for (shift = 6; shift >= 0; shift -= 2) {
				if (xpos + ((6 - shift) / 2) >= info->width) {
					break;
				}
				index = (current_byte >> shift) & 0x03;
				if (!bmp_check_palette_index(im, index)) {
					return 1;
				}
				if (im->open[index]) {
					im->open[index] = 0;
				}
				gdImageSetPixel(im, xpos + ((6 - shift) / 2), row, index);
			}
		}

		for (xpos = padding; xpos > 0; --xpos) {
			if (!gdGetByte(&index, infile)) {
				return 1;
			}
		}
	}
	return 0;
}

static int bmp_read_4bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header)
{
	int ypos = 0, xpos = 0, row = 0, index = 0;
	int padding = 0, current_byte = 0;

	if (info->enctype != BMP_BI_RGB && info->enctype != BMP_BI_RLE4) {
		return 1;
	}

	if (!info->numcolors) {
		info->numcolors = 16;
	} else if (info->numcolors < 0 || info->numcolors > 16) {
		return 1;
	}

	if (bmp_read_palette(im, infile, info->numcolors, (info->type == BMP_PALETTE_4))) {
		return 1;
	}

	im->colorsTotal = info->numcolors;

	/* There is a chance the data isn't until later, would be weird but it is possible */
	if (gdTell(infile) != header->off) {
		/* Should make sure we don't seek past the file size */
		if (!gdSeek(infile, header->off)) {
			return 1;
		}
	}

	if (bmp_row_padding(info->width, info->depth, &padding)) {
		return 1;
	}

	switch (info->enctype) {
	case BMP_BI_RGB:
		for (ypos = 0; ypos < info->height; ++ypos) {
			if (info->topdown) {
				row = ypos;
			} else {
				row = info->height - ypos - 1;
			}

			for (xpos = 0; xpos < info->width; xpos += 2) {
				if (!gdGetByte(&current_byte, infile)) {
					return 1;
				}

				index = (current_byte >> 4) & 0x0f;
				if (!bmp_check_palette_index(im, index)) {
					return 1;
				}
				if (im->open[index]) {
					im->open[index] = 0;
				}
				gdImageSetPixel(im, xpos, row, index);

				/* This condition may get called often, potential optimsations */
				if (xpos + 1 >= info->width) {
					break;
				}

				index = current_byte & 0x0f;
				if (!bmp_check_palette_index(im, index)) {
					return 1;
				}
				if (im->open[index]) {
					im->open[index] = 0;
				}
				gdImageSetPixel(im, xpos + 1, row, index);
			}

			for (xpos = padding; xpos > 0; --xpos) {
				if (!gdGetByte(&index, infile)) {
					return 1;
				}
			}
		}
		break;

	case BMP_BI_RLE4:
		if (bmp_read_rle(im, infile, info)) {
			return 1;
		}
		break;

	default:
		return 1;
	}
	return 0;
}

static int bmp_read_8bit(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info, bmp_hdr_t *header)
{
	int ypos = 0, xpos = 0, row = 0, index = 0;
	int padding = 0;
	int palette_entries = 0;
	int palette_entry_size = (info->type == BMP_PALETTE_4) ? 4 : 3;

	if (info->enctype != BMP_BI_RGB && info->enctype != BMP_BI_RLE8) {
		return 1;
	}

	if (!info->numcolors) {
		info->numcolors = 256;
	} else if (info->numcolors < 0 || info->numcolors > 1024) {
		return 1;
	}

	palette_entries = (header->off - (14 + info->len)) / palette_entry_size;
	if (palette_entries <= 0) {
		return 1;
	}
	if (info->numcolors > palette_entries) {
		info->numcolors = palette_entries;
	}
	if (info->numcolors > gdMaxColors) {
		info->numcolors = gdMaxColors;
	}

	if (bmp_read_palette(im, infile, info->numcolors, (info->type == BMP_PALETTE_4))) {
		return 1;
	}

	im->colorsTotal = info->numcolors;

	/* There is a chance the data isn't until later, would be weird but it is possible */
	if (gdTell(infile) != header->off) {
		/* Should make sure we don't seek past the file size */
		if (!gdSeek(infile, header->off)) {
			return 1;
		}
	}

	if (bmp_row_padding(info->width, info->depth, &padding)) {
		return 1;
	}

	switch (info->enctype) {
	case BMP_BI_RGB:
		for (ypos = 0; ypos < info->height; ++ypos) {
			if (info->topdown) {
				row = ypos;
			} else {
				row = info->height - ypos - 1;
			}

			for (xpos = 0; xpos < info->width; ++xpos) {
				if (!gdGetByte(&index, infile)) {
					return 1;
				}

				if (!bmp_check_palette_index(im, index)) {
					return 1;
				}
				if (im->open[index]) {
					im->open[index] = 0;
				}
				gdImageSetPixel(im, xpos, row, index);
			}
			/* Could create a new variable, but it isn't really worth it */
			for (xpos = padding; xpos > 0; --xpos) {
				if (!gdGetByte(&index, infile)) {
					return 1;
				}
			}
		}
		break;

	case BMP_BI_RLE8:
		if (bmp_read_rle(im, infile, info)) {
			return 1;
		}
		break;

	default:
		return 1;
	}
	return 0;
}

static int bmp_read_rle(gdImagePtr im, gdIOCtxPtr infile, bmp_info_t *info)
{
	int ypos = 0, xpos = 0, row = 0, index = 0;
	int rle_length = 0, rle_data = 0;
	int padding = 0;
	int i = 0, j = 0;
	int pixels_per_byte = 8 / info->depth;

	if (info->topdown) {
		return 1;
	}

	for (ypos = 0; ypos < info->height && xpos <= info->width;) {
		if (!gdGetByte(&rle_length, infile) || !gdGetByte(&rle_data, infile)) {
			return 1;
		}
		row = info->height - ypos - 1;
		if (row < 0 || row >= info->height) {
			return 1;
		}

		if (rle_length != BMP_RLE_COMMAND) {
			for (i = 0; i < rle_length;) {
				for (j = 1; (j <= pixels_per_byte) && (i < rle_length); j++, xpos++, i++) {
					index = (rle_data & (((1 << info->depth) - 1) << (8 - (j * info->depth)))) >> (8 - (j * info->depth));
					if (xpos >= info->width) {
						return 1;
					}
					if (!bmp_check_palette_index(im, index)) {
						return 1;
					}
					if (im->open[index]) {
						im->open[index] = 0;
					}
					gdImageSetPixel(im, xpos, row, index);
				}
			}
		} else if (rle_length == BMP_RLE_COMMAND && rle_data > 2) {
			/* Uncompressed RLE needs to be even */
			padding = 0;
			for (i = 0; i < rle_data; i += pixels_per_byte) {
				int max_pixels = pixels_per_byte;

				if (!gdGetByte(&index, infile)) {
					return 1;
				}
				padding++;

				if (rle_data - i < max_pixels) {
					max_pixels = rle_data - i;
				}

				for (j = 1; j <= max_pixels; j++) {
					int temp = (index >> (8 - (j * info->depth))) & ((1 << info->depth) - 1);
					if (xpos >= info->width) {
						return 1;
					}
					if (!bmp_check_palette_index(im, temp)) {
						return 1;
					}
					if (im->open[temp]) {
						im->open[temp] = 0;
					}
					gdImageSetPixel(im, xpos, row, temp);
					xpos++;
				}
			}

			/* Make sure the bytes read are even */
			if (padding % 2 && !gdGetByte(&index, infile)) {
				return 1;
			}
		} else if (rle_length == BMP_RLE_COMMAND && rle_data == BMP_RLE_ENDOFLINE) {
			/* Next Line */
			xpos = 0;
			ypos++;
		} else if (rle_length == BMP_RLE_COMMAND && rle_data == BMP_RLE_DELTA) {
			/* Delta Record, used for bmp files that contain other data*/
			if (!gdGetByte(&rle_length, infile) || !gdGetByte(&rle_data, infile)) {
				return 1;
			}
			if (xpos + rle_length > info->width || ypos + rle_data >= info->height) {
				return 1;
			}
			xpos += rle_length;
			ypos += rle_data;
		} else if (rle_length == BMP_RLE_COMMAND && rle_data == BMP_RLE_ENDOFBITMAP) {
			/* End of bitmap */
			break;
		}
	}
	return 0;
}
