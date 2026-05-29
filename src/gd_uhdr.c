#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/**
 * File: UltraHDR IO
 *
 * Read and write UltraHDR images with gain map preservation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "gd.h"
#include "gd_errors.h"
#include "gdhelpers.h"

#ifdef HAVE_LIBUHDR
#include <ultrahdr_api.h>
#endif

typedef enum {
	GD_UHDR_OP_RESIZE = 0,
	GD_UHDR_OP_CROP,
	GD_UHDR_OP_ROTATE,
	GD_UHDR_OP_MIRROR
} gdUhdrOpType;

typedef struct {
	gdUhdrOpType type;
	int p1;
	int p2;
	int p3;
	int p4;
} gdUhdrOp;

struct gdUhdrImageStruct {
	int format;
	int width;
	int height;
	int has_gain_map;
	void *blob;
	int blob_size;
	gdUhdrOp *ops;
	int op_count;
	int op_capacity;
};

static void gdUhdrSetError(gdUhdrErrorPtr err, int code, int provider_code, const char *message)
{
	if (!err) {
		return;
	}

	err->code = code;
	err->provider_code = provider_code;
	err->message[0] = '\0';
	if (message) {
		strncpy(err->message, message, sizeof(err->message) - 1);
		err->message[sizeof(err->message) - 1] = '\0';
	}
}

#ifdef HAVE_LIBUHDR

static int gdUhdrIsSupportedFormat(int format)
{
	return format == GD_UHDR_FORMAT_JPEG;
}

static int gdUhdrIntAddOverflow(int a, int b)
{
	if (a < 0 || b < 0) {
		return 1;
	}

	if (a > INT_MAX - b) {
		return 1;
	}

	return 0;
}

static int gdUhdrReadAllFromCtx(gdIOCtxPtr ctx, void **out_data, int *out_size)
{
	const int step = 4096;
	unsigned char buf[4096];
	unsigned char *data = NULL;
	int size = 0;

	if (!ctx || !out_data || !out_size) {
		return GD_UHDR_E_INVALID;
	}

	for (;;) {
		int n = gdGetBuf(buf, step, ctx);
		int new_size;
		if (n <= 0) {
			break;
		}

		if (gdUhdrIntAddOverflow(size, n)) {
			gd_error("gd-uhdr read size overflow: size=%d n=%d\n", size, n);
			gdFree(data);
			return GD_UHDR_E_INVALID;
		}

		new_size = size + n;

		{
			unsigned char *tmp = (unsigned char *) gdRealloc(data, (size_t) new_size);
			if (!tmp) {
				gd_error("gd-uhdr realloc failed: requested=%d\n", new_size);
				gdFree(data);
				return GD_UHDR_E_DECODE;
			}
			data = tmp;
		}

		memcpy(data + size, buf, (size_t) n);
		size = new_size;
	}

	if (!data || size <= 0) {
		gdFree(data);
		return GD_UHDR_E_DECODE;
	}

	*out_data = data;
	*out_size = size;
	return GD_UHDR_SUCCESS;
}

static int gdUhdrApplyOps(uhdr_codec_private_t *codec, gdUhdrImagePtr im, gdUhdrErrorPtr err)
{
	int i;

	for (i = 0; i < im->op_count; i++) {
		uhdr_error_info_t rc;
		gdUhdrOp *op = &im->ops[i];

		switch (op->type) {
			case GD_UHDR_OP_RESIZE:
				rc = uhdr_add_effect_resize(codec, op->p1, op->p2);
				break;
			case GD_UHDR_OP_CROP:
				rc = uhdr_add_effect_crop(codec, op->p1, op->p2, op->p3, op->p4);
				break;
			case GD_UHDR_OP_ROTATE:
				rc = uhdr_add_effect_rotate(codec, op->p1);
				break;
			case GD_UHDR_OP_MIRROR:
				rc = uhdr_add_effect_mirror(codec,
					op->p1 == GD_UHDR_MIRROR_HORIZONTAL ? UHDR_MIRROR_HORIZONTAL : UHDR_MIRROR_VERTICAL);
				break;
			default:
				gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Unknown queued UltraHDR operation");
				return GD_UHDR_E_INVALID;
		}

		if (rc.error_code != UHDR_CODEC_OK) {
			gdUhdrSetError(err, GD_UHDR_E_INVALID, rc.error_code, rc.has_detail ? rc.detail : "UltraHDR effect failed");
			return GD_UHDR_E_INVALID;
		}
	}

	return GD_UHDR_SUCCESS;
}

static int gdUhdrBytesPerPixel(uhdr_img_fmt_t fmt)
{
	switch (fmt) {
		case UHDR_IMG_FMT_32bppRGBA8888:
		case UHDR_IMG_FMT_32bppRGBA1010102:
			return 4;
		case UHDR_IMG_FMT_64bppRGBAHalfFloat:
			return 8;
		default:
			return 0;
	}
}

static int gdUhdrCopyDecodedPackedImage(uhdr_raw_image_t *dst, uhdr_raw_image_t *src, gdUhdrErrorPtr err)
{
	unsigned int y;
	unsigned int src_stride;
	int bpp;
	size_t row_bytes;
	size_t total_bytes;
	unsigned char *buf;

	if (!dst || !src || !src->planes[UHDR_PLANE_PACKED] || src->w == 0 || src->h == 0) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Invalid decoded image for copy");
		return GD_UHDR_E_DECODE;
	}

	bpp = gdUhdrBytesPerPixel(src->fmt);
	if (bpp == 0) {
		gdUhdrSetError(err, GD_UHDR_E_UNSUPPORTED, 0, "Unsupported decoded pixel format");
		return GD_UHDR_E_UNSUPPORTED;
	}

	if (src->w > (unsigned int) (INT_MAX / bpp)) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Decoded row size overflow");
		return GD_UHDR_E_INVALID;
	}

	row_bytes = (size_t) src->w * (size_t) bpp;
	total_bytes = row_bytes * (size_t) src->h;
	buf = (unsigned char *) gdMalloc(total_bytes);
	if (!buf) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Out of memory copying decoded image");
		return GD_UHDR_E_DECODE;
	}

	src_stride = src->stride[UHDR_PLANE_PACKED] ? src->stride[UHDR_PLANE_PACKED] : src->w;
	for (y = 0; y < src->h; y++) {
		unsigned char *dst_row = buf + ((size_t) y * row_bytes);
		unsigned char *src_row = (unsigned char *) src->planes[UHDR_PLANE_PACKED] +
			((size_t) y * (size_t) src_stride * (size_t) bpp);
		memcpy(dst_row, src_row, row_bytes);
	}

	memset(dst, 0, sizeof(*dst));
	dst->fmt = src->fmt;
	dst->cg = src->cg;
	dst->ct = src->ct;
	dst->range = UHDR_CR_FULL_RANGE;
	dst->w = src->w;
	dst->h = src->h;
	dst->planes[UHDR_PLANE_PACKED] = buf;
	dst->stride[UHDR_PLANE_PACKED] = src->w;

	return GD_UHDR_SUCCESS;
}

static void gdUhdrFreeCopiedRawImage(uhdr_raw_image_t *img)
{
	if (!img) {
		return;
	}

	gdFree(img->planes[UHDR_PLANE_PACKED]);
	memset(img, 0, sizeof(*img));
}

static int gdUhdrDecodeRawIntent(gdUhdrImagePtr im, uhdr_img_fmt_t fmt, uhdr_color_transfer_t ct,
	uhdr_raw_image_t *out, gdUhdrErrorPtr err)
{
	uhdr_codec_private_t *dec;
	uhdr_compressed_image_t source;
	uhdr_error_info_t rc;
	uhdr_raw_image_t *decoded;
	int status;

	dec = uhdr_create_decoder();
	if (!dec) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Failed to create UltraHDR decoder");
		return GD_UHDR_E_DECODE;
	}

	memset(&source, 0, sizeof(source));
	source.data = im->blob;
	source.data_sz = (size_t) im->blob_size;
	source.capacity = (size_t) im->blob_size;
	source.cg = UHDR_CG_UNSPECIFIED;
	source.ct = UHDR_CT_UNSPECIFIED;
	source.range = UHDR_CR_FULL_RANGE;

	rc = uhdr_dec_set_image(dec, &source);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_dec_set_image failed");
		uhdr_release_decoder(dec);
		return GD_UHDR_E_DECODE;
	}

	rc = uhdr_dec_set_out_img_format(dec, fmt);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_dec_set_out_img_format failed");
		uhdr_release_decoder(dec);
		return GD_UHDR_E_DECODE;
	}

	rc = uhdr_dec_set_out_color_transfer(dec, ct);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_dec_set_out_color_transfer failed");
		uhdr_release_decoder(dec);
		return GD_UHDR_E_DECODE;
	}

	status = gdUhdrApplyOps(dec, im, err);
	if (status != GD_UHDR_SUCCESS) {
		uhdr_release_decoder(dec);
		return status;
	}

	rc = uhdr_decode(dec);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_decode failed");
		uhdr_release_decoder(dec);
		return GD_UHDR_E_DECODE;
	}

	decoded = uhdr_get_decoded_image(dec);
	status = gdUhdrCopyDecodedPackedImage(out, decoded, err);
	uhdr_release_decoder(dec);
	return status;
}

static int gdUhdrQueueOp(gdUhdrImagePtr im, gdUhdrOpType type, int p1, int p2, int p3, int p4)
{
	if (im->op_count == im->op_capacity) {
		int new_cap = im->op_capacity == 0 ? 8 : im->op_capacity * 2;
		gdUhdrOp *tmp;

		if (overflow2(new_cap, (int) sizeof(gdUhdrOp))) {
			return GD_UHDR_E_INVALID;
		}

		tmp = (gdUhdrOp *) gdRealloc(im->ops, (size_t) new_cap * sizeof(gdUhdrOp));
		if (!tmp) {
			return GD_UHDR_E_INVALID;
		}

		im->ops = tmp;
		im->op_capacity = new_cap;
	}

	im->ops[im->op_count].type = type;
	im->ops[im->op_count].p1 = p1;
	im->ops[im->op_count].p2 = p2;
	im->ops[im->op_count].p3 = p3;
	im->ops[im->op_count].p4 = p4;
	im->op_count++;

	return GD_UHDR_SUCCESS;
}

static gdUhdrImagePtr gdUhdrImageCreateFromData(void *data, int size, int format, gdUhdrErrorPtr err)
{
	gdUhdrImagePtr im;
	uhdr_codec_private_t *dec;
	uhdr_compressed_image_t input;
	uhdr_error_info_t rc;

	if (!data || size <= 0 || !gdUhdrIsSupportedFormat(format)) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Invalid UltraHDR input");
		return NULL;
	}

	if (!is_uhdr_image(data, size)) {
		gdUhdrSetError(err, GD_UHDR_E_UNSUPPORTED, 0, "Input is not a valid UltraHDR image");
		return NULL;
	}

	dec = uhdr_create_decoder();
	if (!dec) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Failed to create UltraHDR decoder");
		return NULL;
	}

	input.data = data;
	input.data_sz = (size_t) size;
	input.capacity = (size_t) size;
	input.cg = UHDR_CG_UNSPECIFIED;
	input.ct = UHDR_CT_UNSPECIFIED;
	input.range = UHDR_CR_FULL_RANGE;

	rc = uhdr_dec_set_image(dec, &input);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_dec_set_image failed");
		uhdr_release_decoder(dec);
		return NULL;
	}

	rc = uhdr_dec_probe(dec);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_dec_probe failed");
		uhdr_release_decoder(dec);
		return NULL;
	}

	im = (gdUhdrImagePtr) gdCalloc(1, sizeof(*im));
	if (!im) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Out of memory");
		uhdr_release_decoder(dec);
		return NULL;
	}

	im->blob = gdMalloc((size_t) size);
	if (!im->blob) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Out of memory");
		gdFree(im);
		uhdr_release_decoder(dec);
		return NULL;
	}

	memcpy(im->blob, data, (size_t) size);
	im->blob_size = size;
	im->format = format;
	im->width = uhdr_dec_get_image_width(dec);
	im->height = uhdr_dec_get_image_height(dec);
	im->has_gain_map = (uhdr_dec_get_gainmap_width(dec) > 0 && uhdr_dec_get_gainmap_height(dec) > 0) ? 1 : 0;

	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	uhdr_release_decoder(dec);
	return im;
}

#endif

#ifndef HAVE_LIBUHDR

static int gdUhdrUnavailableCode(void)
{
	return GD_UHDR_NOT_AVAILABLE;
}

static const char *gdUhdrUnavailableMessage(void)
{
	return "UltraHDR support is not enabled in this build";
}

#endif

/*
	Function: gdUhdrIsAvailable

		Returns whether UltraHDR support is available in this libgd build.

	Returns:

		1 if available, 0 otherwise.
*/
BGD_DECLARE(int) gdUhdrIsAvailable(void)
{
#ifdef HAVE_LIBUHDR
	return 1;
#else
	return 0;
#endif
}

/*
	Function: gdUhdrImageCreateFromFile

		Loads an UltraHDR image from a file.

	Variants:

		<gdUhdrImageCreateFromCtx> loads from a <gdIOCtx>.

		<gdUhdrImageCreateFromPtr> loads from memory.

	Parameters:

		filename - input file path.
		format   - input format (currently <GD_UHDR_FORMAT_JPEG>).
		err      - optional output error details.

	Returns:

		A new <gdUhdrImagePtr> on success, or NULL on error.
*/
BGD_DECLARE(gdUhdrImagePtr) gdUhdrImageCreateFromFile(const char *filename, int format, gdUhdrErrorPtr err)
{
#ifdef HAVE_LIBUHDR
	FILE *fp;
	gdIOCtxPtr in;
	gdUhdrImagePtr im;

	if (!filename) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "filename must not be NULL");
		return NULL;
	}

	fp = fopen(filename, "rb");
	if (!fp) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Failed to open input file");
		return NULL;
	}

	in = gdNewFileCtx(fp);
	if (!in) {
		fclose(fp);
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Failed to create IO context");
		return NULL;
	}

	im = gdUhdrImageCreateFromCtx(in, format, err);
	in->gd_free(in);
	fclose(fp);
	return im;
#else
	ARG_NOT_USED(filename);
	ARG_NOT_USED(format);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return NULL;
#endif
}

/*
	Function: gdUhdrImageCreateFromCtx

		See <gdUhdrImageCreateFromFile>.
*/
BGD_DECLARE(gdUhdrImagePtr) gdUhdrImageCreateFromCtx(gdIOCtxPtr ctx, int format, gdUhdrErrorPtr err)
{
#ifdef HAVE_LIBUHDR
	void *data = NULL;
	int size = 0;
	int rc;
	gdUhdrImagePtr im;

	if (!gdUhdrIsSupportedFormat(format)) {
		gdUhdrSetError(err, GD_UHDR_E_UNSUPPORTED, 0, "Unsupported UltraHDR format");
		return NULL;
	}

	rc = gdUhdrReadAllFromCtx(ctx, &data, &size);
	if (rc != GD_UHDR_SUCCESS) {
		gdUhdrSetError(err, rc, 0, "Failed to read UltraHDR stream");
		return NULL;
	}

	im = gdUhdrImageCreateFromData(data, size, format, err);
	gdFree(data);
	return im;
#else
	ARG_NOT_USED(ctx);
	ARG_NOT_USED(format);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return NULL;
#endif
}

/*
	Function: gdUhdrImageCreateFromPtr

		See <gdUhdrImageCreateFromFile>.
*/
BGD_DECLARE(gdUhdrImagePtr) gdUhdrImageCreateFromPtr(int size, void *data, int format, gdUhdrErrorPtr err)
{
#ifdef HAVE_LIBUHDR
	if (!gdUhdrIsSupportedFormat(format)) {
		gdUhdrSetError(err, GD_UHDR_E_UNSUPPORTED, 0, "Unsupported UltraHDR format");
		return NULL;
	}

	return gdUhdrImageCreateFromData(data, size, format, err);
#else
	ARG_NOT_USED(size);
	ARG_NOT_USED(data);
	ARG_NOT_USED(format);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return NULL;
#endif
}

/*
	Function: gdUhdrImageDestroy

		Releases an UltraHDR image created by <gdUhdrImageCreateFromFile> or its variants.
*/
BGD_DECLARE(void) gdUhdrImageDestroy(gdUhdrImagePtr im)
{
	if (!im) {
		return;
	}

	gdFree(im->ops);
	gdFree(im->blob);
	gdFree(im);
}

/*
  Function: gdUhdrImageWidth

    Returns the image width in pixels.
*/
BGD_DECLARE(int) gdUhdrImageWidth(gdUhdrImagePtr im)
{
	if (!im) {
		return 0;
	}
	return im->width;
}

/*
  Function: gdUhdrImageHeight

    Returns the image height in pixels.
*/
BGD_DECLARE(int) gdUhdrImageHeight(gdUhdrImagePtr im)
{
	if (!im) {
		return 0;
	}
	return im->height;
}

/*
  Function: gdUhdrImageHasGainMap

    Returns nonzero when the loaded image contains a gain map.
*/
BGD_DECLARE(int) gdUhdrImageHasGainMap(gdUhdrImagePtr im)
{
	if (!im) {
		return 0;
	}
	return im->has_gain_map;
}

/*
  Function: gdUhdrImageResize

    Queues a resize operation to be applied at save time.
*/
BGD_DECLARE(int) gdUhdrImageResize(gdUhdrImagePtr im, int width, int height, gdUhdrErrorPtr err)
{
#ifdef HAVE_LIBUHDR
	int rc;
	if (!im || width <= 0 || height <= 0) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Invalid resize arguments");
		return GD_UHDR_E_INVALID;
	}

	rc = gdUhdrQueueOp(im, GD_UHDR_OP_RESIZE, width, height, 0, 0);
	if (rc != GD_UHDR_SUCCESS) {
		gdUhdrSetError(err, rc, 0, "Failed to queue resize effect");
		return rc;
	}

	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	return GD_UHDR_SUCCESS;
#else
	ARG_NOT_USED(im);
	ARG_NOT_USED(width);
	ARG_NOT_USED(height);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return gdUhdrUnavailableCode();
#endif
}

/*
	Function: gdUhdrImageCrop

		Queues a crop operation to be applied at save time.
*/
BGD_DECLARE(int) gdUhdrImageCrop(gdUhdrImagePtr im, int left, int top, int width, int height, gdUhdrErrorPtr err)
{
#ifdef HAVE_LIBUHDR
	int rc;
	int right;
	int bottom;

	if (!im || left < 0 || top < 0 || width <= 0 || height <= 0) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Invalid crop arguments");
		return GD_UHDR_E_INVALID;
	}

	right = left + width - 1;
	bottom = top + height - 1;

	rc = gdUhdrQueueOp(im, GD_UHDR_OP_CROP, left, right, top, bottom);
	if (rc != GD_UHDR_SUCCESS) {
		gdUhdrSetError(err, rc, 0, "Failed to queue crop effect");
		return rc;
	}

	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	return GD_UHDR_SUCCESS;
#else
	ARG_NOT_USED(im);
	ARG_NOT_USED(left);
	ARG_NOT_USED(top);
	ARG_NOT_USED(width);
	ARG_NOT_USED(height);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return gdUhdrUnavailableCode();
#endif
}

/*
	Function: gdUhdrImageRotate

		Queues a clockwise rotation (90, 180, or 270 degrees) to be applied at save time.
*/
BGD_DECLARE(int) gdUhdrImageRotate(gdUhdrImagePtr im, int degrees, gdUhdrErrorPtr err)
{
#ifdef HAVE_LIBUHDR
	int rc;
	if (!im || (degrees != 90 && degrees != 180 && degrees != 270)) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Invalid rotation angle");
		return GD_UHDR_E_INVALID;
	}

	rc = gdUhdrQueueOp(im, GD_UHDR_OP_ROTATE, degrees, 0, 0, 0);
	if (rc != GD_UHDR_SUCCESS) {
		gdUhdrSetError(err, rc, 0, "Failed to queue rotation effect");
		return rc;
	}

	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	return GD_UHDR_SUCCESS;
#else
	ARG_NOT_USED(im);
	ARG_NOT_USED(degrees);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return gdUhdrUnavailableCode();
#endif
}

/*
	Function: gdUhdrImageMirror

		Queues a mirror operation to be applied at save time.
*/
BGD_DECLARE(int) gdUhdrImageMirror(gdUhdrImagePtr im, int axis, gdUhdrErrorPtr err)
{
#ifdef HAVE_LIBUHDR
	int rc;
	if (!im || (axis != GD_UHDR_MIRROR_HORIZONTAL && axis != GD_UHDR_MIRROR_VERTICAL)) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Invalid mirror axis");
		return GD_UHDR_E_INVALID;
	}

	rc = gdUhdrQueueOp(im, GD_UHDR_OP_MIRROR, axis, 0, 0, 0);
	if (rc != GD_UHDR_SUCCESS) {
		gdUhdrSetError(err, rc, 0, "Failed to queue mirror effect");
		return rc;
	}

	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	return GD_UHDR_SUCCESS;
#else
	ARG_NOT_USED(im);
	ARG_NOT_USED(axis);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return gdUhdrUnavailableCode();
#endif
}

/*
	Function: gdUhdrImageFile

		Saves an UltraHDR image to a file path.

	Variants:

		<gdUhdrImageCtx> writes via <gdIOCtx>.

		<gdUhdrImageWritePtr> writes to memory.
*/
BGD_DECLARE(int) gdUhdrImageFile(gdUhdrImagePtr im, const char *filename, int format, int quality, gdUhdrErrorPtr err)
{
#ifdef HAVE_LIBUHDR
	FILE *fp;
	gdIOCtxPtr out;
	int rc;

	if (!filename) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "filename must not be NULL");
		return GD_UHDR_E_INVALID;
	}

	fp = fopen(filename, "wb");
	if (!fp) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0, "Failed to open output file");
		return GD_UHDR_E_ENCODE;
	}

	out = gdNewFileCtx(fp);
	if (!out) {
		fclose(fp);
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0, "Failed to create IO context");
		return GD_UHDR_E_ENCODE;
	}

	rc = gdUhdrImageCtx(im, out, format, quality, err);
	out->gd_free(out);
	fclose(fp);
	return rc;
#else
	ARG_NOT_USED(im);
	ARG_NOT_USED(filename);
	ARG_NOT_USED(format);
	ARG_NOT_USED(quality);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return gdUhdrUnavailableCode();
#endif
}

/*
	Function: gdUhdrImageCtx

		See <gdUhdrImageFile>.
*/
BGD_DECLARE(int) gdUhdrImageCtx(gdUhdrImagePtr im, gdIOCtxPtr ctx, int format, int quality, gdUhdrErrorPtr err)
{
#ifdef HAVE_LIBUHDR
	uhdr_codec_private_t *dec;
	uhdr_codec_private_t *enc;
	uhdr_compressed_image_t source;
	uhdr_error_info_t rc;
	uhdr_mem_block_t *base;
	uhdr_mem_block_t *gain;
	uhdr_gainmap_metadata_t *meta;
	uhdr_mem_block_t *exif;
	uhdr_compressed_image_t base_image;
	uhdr_compressed_image_t gain_image;
	uhdr_mem_block_t exif_block;
	uhdr_compressed_image_t *encoded;
	uhdr_raw_image_t raw_sdr;
	uhdr_raw_image_t raw_hdr;
	int use_raw_intents;
	int write_result;

	if (!im || !ctx || !im->blob || im->blob_size <= 0) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Invalid UltraHDR image or output context");
		return GD_UHDR_E_INVALID;
	}

	if (!gdUhdrIsSupportedFormat(format)) {
		gdUhdrSetError(err, GD_UHDR_E_UNSUPPORTED, 0, "Unsupported UltraHDR output format");
		return GD_UHDR_E_UNSUPPORTED;
	}

	if (quality < 1 || quality > 95) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "quality must be in [1..95]");
		return GD_UHDR_E_INVALID;
	}

	memset(&raw_sdr, 0, sizeof(raw_sdr));
	memset(&raw_hdr, 0, sizeof(raw_hdr));
	use_raw_intents = im->op_count > 0;

	dec = uhdr_create_decoder();
	enc = uhdr_create_encoder();
	if (!dec || !enc) {
		if (dec) {
			uhdr_release_decoder(dec);
		}
		if (enc) {
			uhdr_release_encoder(enc);
		}
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0, "Failed to create UltraHDR codec contexts");
		return GD_UHDR_E_ENCODE;
	}

	source.data = im->blob;
	source.data_sz = (size_t) im->blob_size;
	source.capacity = (size_t) im->blob_size;
	source.cg = UHDR_CG_UNSPECIFIED;
	source.ct = UHDR_CT_UNSPECIFIED;
	source.range = UHDR_CR_FULL_RANGE;

	rc = uhdr_dec_set_image(dec, &source);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_dec_set_image failed");
		uhdr_release_encoder(enc);
		uhdr_release_decoder(dec);
		return GD_UHDR_E_DECODE;
	}

	rc = uhdr_dec_probe(dec);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_dec_probe failed");
		uhdr_release_encoder(enc);
		uhdr_release_decoder(dec);
		return GD_UHDR_E_DECODE;
	}

	if (use_raw_intents) {
		int status;

		status = gdUhdrDecodeRawIntent(im, UHDR_IMG_FMT_32bppRGBA8888, UHDR_CT_SRGB, &raw_sdr, err);
		if (status != GD_UHDR_SUCCESS) {
			uhdr_release_encoder(enc);
			uhdr_release_decoder(dec);
			return status;
		}
		raw_sdr.ct = UHDR_CT_SRGB;
		raw_sdr.range = UHDR_CR_FULL_RANGE;

		status = gdUhdrDecodeRawIntent(im, UHDR_IMG_FMT_32bppRGBA1010102, UHDR_CT_PQ, &raw_hdr, err);
		if (status != GD_UHDR_SUCCESS) {
			gdUhdrFreeCopiedRawImage(&raw_sdr);
			uhdr_release_encoder(enc);
			uhdr_release_decoder(dec);
			return status;
		}
		raw_hdr.ct = UHDR_CT_PQ;
		raw_hdr.range = UHDR_CR_FULL_RANGE;

		rc = uhdr_enc_set_raw_image(enc, &raw_hdr, UHDR_HDR_IMG);
		if (rc.error_code != UHDR_CODEC_OK) {
			gdUhdrSetError(err, GD_UHDR_E_ENCODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_enc_set_raw_image(HDR) failed");
			gdUhdrFreeCopiedRawImage(&raw_hdr);
			gdUhdrFreeCopiedRawImage(&raw_sdr);
			uhdr_release_encoder(enc);
			uhdr_release_decoder(dec);
			return GD_UHDR_E_ENCODE;
		}

		rc = uhdr_enc_set_raw_image(enc, &raw_sdr, UHDR_SDR_IMG);
		if (rc.error_code != UHDR_CODEC_OK) {
			gdUhdrSetError(err, GD_UHDR_E_ENCODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_enc_set_raw_image(SDR) failed");
			gdUhdrFreeCopiedRawImage(&raw_hdr);
			gdUhdrFreeCopiedRawImage(&raw_sdr);
			uhdr_release_encoder(enc);
			uhdr_release_decoder(dec);
			return GD_UHDR_E_ENCODE;
		}
	} else {
		base = uhdr_dec_get_base_image(dec);
		gain = uhdr_dec_get_gainmap_image(dec);
		meta = uhdr_dec_get_gainmap_metadata(dec);
		if (!base || !gain || !meta) {
			gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Decoded stream is missing base image, gain map, or metadata");
			uhdr_release_encoder(enc);
			uhdr_release_decoder(dec);
			return GD_UHDR_E_DECODE;
		}

		base_image.data = base->data;
		base_image.data_sz = base->data_sz;
		base_image.capacity = base->data_sz;
		base_image.cg = UHDR_CG_UNSPECIFIED;
		base_image.ct = UHDR_CT_UNSPECIFIED;
		base_image.range = UHDR_CR_FULL_RANGE;

		gain_image.data = gain->data;
		gain_image.data_sz = gain->data_sz;
		gain_image.capacity = gain->data_sz;
		gain_image.cg = UHDR_CG_UNSPECIFIED;
		gain_image.ct = UHDR_CT_UNSPECIFIED;
		gain_image.range = UHDR_CR_FULL_RANGE;

		rc = uhdr_enc_set_compressed_image(enc, &base_image, UHDR_BASE_IMG);
		if (rc.error_code != UHDR_CODEC_OK) {
			gdUhdrSetError(err, GD_UHDR_E_ENCODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_enc_set_compressed_image failed");
			uhdr_release_encoder(enc);
			uhdr_release_decoder(dec);
			return GD_UHDR_E_ENCODE;
		}

		rc = uhdr_enc_set_gainmap_image(enc, &gain_image, meta);
		if (rc.error_code != UHDR_CODEC_OK) {
			gdUhdrSetError(err, GD_UHDR_E_ENCODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_enc_set_gainmap_image failed");
			uhdr_release_encoder(enc);
			uhdr_release_decoder(dec);
			return GD_UHDR_E_ENCODE;
		}
	}

	rc = uhdr_enc_set_quality(enc, quality, UHDR_BASE_IMG);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_enc_set_quality(base) failed");
		uhdr_release_encoder(enc);
		uhdr_release_decoder(dec);
		return GD_UHDR_E_ENCODE;
	}

	rc = uhdr_enc_set_quality(enc, quality, UHDR_GAIN_MAP_IMG);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_enc_set_quality(gainmap) failed");
		uhdr_release_encoder(enc);
		uhdr_release_decoder(dec);
		return GD_UHDR_E_ENCODE;
	}

	exif = uhdr_dec_get_exif(dec);
	if (exif && exif->data && exif->data_sz > 0) {
		exif_block.data = exif->data;
		exif_block.data_sz = exif->data_sz;
		exif_block.capacity = exif->data_sz;
		rc = uhdr_enc_set_exif_data(enc, &exif_block);
		if (rc.error_code != UHDR_CODEC_OK) {
			gdUhdrSetError(err, GD_UHDR_E_ENCODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_enc_set_exif_data failed");
			uhdr_release_encoder(enc);
			uhdr_release_decoder(dec);
			return GD_UHDR_E_ENCODE;
		}
	}

	rc = uhdr_enc_set_output_format(enc, UHDR_CODEC_JPG);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_enc_set_output_format failed");
		uhdr_release_encoder(enc);
		uhdr_release_decoder(dec);
		return GD_UHDR_E_ENCODE;
	}

	if (!use_raw_intents && gdUhdrApplyOps(enc, im, err) != GD_UHDR_SUCCESS) {
		uhdr_release_encoder(enc);
		uhdr_release_decoder(dec);
		return GD_UHDR_E_INVALID;
	}

	rc = uhdr_encode(enc);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_encode failed");
		gdUhdrFreeCopiedRawImage(&raw_hdr);
		gdUhdrFreeCopiedRawImage(&raw_sdr);
		uhdr_release_encoder(enc);
		uhdr_release_decoder(dec);
		return GD_UHDR_E_ENCODE;
	}

	encoded = uhdr_get_encoded_stream(enc);
	if (!encoded || !encoded->data || encoded->data_sz == 0) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0, "Encoded UltraHDR stream is empty");
		gdUhdrFreeCopiedRawImage(&raw_hdr);
		gdUhdrFreeCopiedRawImage(&raw_sdr);
		uhdr_release_encoder(enc);
		uhdr_release_decoder(dec);
		return GD_UHDR_E_ENCODE;
	}

	write_result = gdPutBuf(encoded->data, (int) encoded->data_sz, ctx);
	if (write_result != (int) encoded->data_sz) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0, "Failed to write UltraHDR output");
		gdUhdrFreeCopiedRawImage(&raw_hdr);
		gdUhdrFreeCopiedRawImage(&raw_sdr);
		uhdr_release_encoder(enc);
		uhdr_release_decoder(dec);
		return GD_UHDR_E_ENCODE;
	}

	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	gdUhdrFreeCopiedRawImage(&raw_hdr);
	gdUhdrFreeCopiedRawImage(&raw_sdr);
	uhdr_release_encoder(enc);
	uhdr_release_decoder(dec);
	return GD_UHDR_SUCCESS;
#else
	ARG_NOT_USED(im);
	ARG_NOT_USED(ctx);
	ARG_NOT_USED(format);
	ARG_NOT_USED(quality);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return gdUhdrUnavailableCode();
#endif
}

/*
	Function: gdUhdrImageWritePtr

		See <gdUhdrImageFile>.
*/
BGD_DECLARE(void *) gdUhdrImageWritePtr(gdUhdrImagePtr im, int *size, int format, int quality, gdUhdrErrorPtr err)
{
#ifdef HAVE_LIBUHDR
	gdIOCtxPtr out;
	void *rv;

	if (size) {
		*size = 0;
	}

	out = gdNewDynamicCtx(2048, NULL);
	if (!out) {
		gdUhdrSetError(err, GD_UHDR_E_ENCODE, 0, "Failed to create dynamic output context");
		return NULL;
	}

	if (gdUhdrImageCtx(im, out, format, quality, err) != GD_UHDR_SUCCESS) {
		out->gd_free(out);
		return NULL;
	}

	rv = gdDPExtractData(out, size);
	out->gd_free(out);
	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	return rv;
#else
	ARG_NOT_USED(im);
	ARG_NOT_USED(format);
	ARG_NOT_USED(quality);
	if (size) {
		*size = 0;
	}
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return NULL;
#endif
}

/*
	Function: gdUhdrImageGetSdr

		Decodes and returns the SDR base image as a <gdImagePtr>.
*/
BGD_DECLARE(gdImagePtr) gdUhdrImageGetSdr(gdUhdrImagePtr im, gdUhdrErrorPtr err)
{
#ifdef HAVE_LIBUHDR
	uhdr_codec_private_t *dec;
	uhdr_compressed_image_t input;
	uhdr_error_info_t rc;
	uhdr_raw_image_t *raw;
	gdImagePtr out;
	int x, y;
	unsigned int stride;
	unsigned char *row;

	if (!im || !im->blob || im->blob_size <= 0) {
		gdUhdrSetError(err, GD_UHDR_E_INVALID, 0, "Invalid UltraHDR image");
		return NULL;
	}

	dec = uhdr_create_decoder();
	if (!dec) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Failed to create UltraHDR decoder");
		return NULL;
	}

	input.data = im->blob;
	input.data_sz = (size_t) im->blob_size;
	input.capacity = (size_t) im->blob_size;
	input.cg = UHDR_CG_UNSPECIFIED;
	input.ct = UHDR_CT_UNSPECIFIED;
	input.range = UHDR_CR_FULL_RANGE;

	rc = uhdr_dec_set_image(dec, &input);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_dec_set_image failed");
		uhdr_release_decoder(dec);
		return NULL;
	}

	rc = uhdr_dec_set_out_img_format(dec, UHDR_IMG_FMT_32bppRGBA8888);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_dec_set_out_img_format failed");
		uhdr_release_decoder(dec);
		return NULL;
	}

	rc = uhdr_dec_set_out_color_transfer(dec, UHDR_CT_SRGB);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_dec_set_out_color_transfer failed");
		uhdr_release_decoder(dec);
		return NULL;
	}

	rc = uhdr_decode(dec);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, rc.error_code, rc.has_detail ? rc.detail : "uhdr_decode failed");
		uhdr_release_decoder(dec);
		return NULL;
	}

	raw = uhdr_get_decoded_image(dec);
	if (!raw || !raw->planes[UHDR_PLANE_PACKED] || raw->w == 0 || raw->h == 0) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Decoded SDR image unavailable");
		uhdr_release_decoder(dec);
		return NULL;
	}

	out = gdImageCreateTrueColor((int) raw->w, (int) raw->h);
	if (!out) {
		gdUhdrSetError(err, GD_UHDR_E_DECODE, 0, "Failed to allocate SDR gdImage");
		uhdr_release_decoder(dec);
		return NULL;
	}

	stride = raw->stride[UHDR_PLANE_PACKED] ? raw->stride[UHDR_PLANE_PACKED] : raw->w;
	for (y = 0; y < (int) raw->h; y++) {
		row = (unsigned char *) raw->planes[UHDR_PLANE_PACKED] + ((size_t) y * stride * 4);
		for (x = 0; x < (int) raw->w; x++) {
			unsigned char r = row[(size_t) x * 4 + 0];
			unsigned char g = row[(size_t) x * 4 + 1];
			unsigned char b = row[(size_t) x * 4 + 2];
			unsigned char a8 = row[(size_t) x * 4 + 3];
			int a7 = gdAlphaMax - (a8 >> 1);
			out->tpixels[y][x] = gdTrueColorAlpha(r, g, b, a7);
		}
	}
	out->saveAlphaFlag = 1;

	gdUhdrSetError(err, GD_UHDR_SUCCESS, 0, NULL);
	uhdr_release_decoder(dec);
	return out;
#else
	ARG_NOT_USED(im);
	gdUhdrSetError(err, gdUhdrUnavailableCode(), 0, gdUhdrUnavailableMessage());
	return NULL;
#endif
}
