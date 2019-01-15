/*
 * gd_jpeg.c: Read and write JPEG (JFIF) format image files using the
 * gd graphics library (http://www.libgd.org).
 *
 * This software is based in part on the work of the Independent JPEG
 * Group.  For more information on the IJG JPEG software (and JPEG
 * documentation, etc.), see ftp://ftp.uu.net/graphics/jpeg/.
 *
 * NOTE: IJG 12-bit JSAMPLE (BITS_IN_JSAMPLE == 12) mode is not
 * supported at all on read in gd 2.0, and is not supported on write
 * except for palette images, which is sort of pointless (TBB). Even that
 * has never been tested according to DB.
 *
 * Copyright 2000 Doug Becker, mailto:thebeckers@home.com
 *
 * Modification 4/18/00 TBB: JPEG_DEBUG rather than just DEBUG,
 * so VC++ builds don't spew to standard output, causing
 * major CGI brain damage
 *
 * 2.0.10: more efficient gdImageCreateFromJpegCtx, thanks to
 * Christian Aberger
 */

/**
 * File: JPEG IO
 *
 * Read and write JPEG images.
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <stdlib.h>
#include <setjmp.h>
#include <limits.h>
#include <string.h>

#include "gd.h"
#include "gd_errors.h"
/* TBB: move this up so include files are not brought in */
/* JCE: arrange HAVE_LIBJPEG so that it can be set in gd.h */
#ifdef HAVE_LIBJPEG
#include "gdhelpers.h"

#if defined(_WIN32) && defined(__MINGW32__)
# define HAVE_BOOLEAN
#endif

/* 1.8.1: remove dependency on jinclude.h */
#include "jpeglib.h"
#include "jerror.h"

static const char *const GD_JPEG_VERSION = "1.0";

typedef struct _jmpbuf_wrapper {
	jmp_buf jmpbuf;
        int ignore_warning;
}
jmpbuf_wrapper;

static void jpeg_emit_message(j_common_ptr jpeg_info, int level)
{
	char message[JMSG_LENGTH_MAX];
	jmpbuf_wrapper *jmpbufw;
	int ignore_warning = 0;

	jmpbufw = (jmpbuf_wrapper *) jpeg_info->client_data;

	if (jmpbufw != 0) {
		ignore_warning = jmpbufw->ignore_warning;
	}

	(jpeg_info->err->format_message)(jpeg_info,message);

	/* It is a warning message */
	if (level < 0) {
		/* display only the 1st warning, as would do a default libjpeg
		 * unless strace_level >= 3
		 */
		if ((jpeg_info->err->num_warnings == 0) || (jpeg_info->err->trace_level >= 3)) {
			if (!ignore_warning) {
				gd_error("gd-jpeg, libjpeg: recoverable error: %s\n", message);
			}
		}

		jpeg_info->err->num_warnings++;
	} else {
		/* strace msg, Show it if trace_level >= level. */
		if (jpeg_info->err->trace_level >= level) {
			if (!ignore_warning) {
				gd_error("gd-jpeg, libjpeg: strace message: %s\n", message);
			}
		}
	}
}

/* Called by the IJG JPEG library upon encountering a fatal error */
static void fatal_jpeg_error(j_common_ptr cinfo)
{
	jmpbuf_wrapper *jmpbufw;
	char buffer[JMSG_LENGTH_MAX];

	(*cinfo->err->format_message)(cinfo, buffer);
	gd_error_ex(GD_WARNING, "gd-jpeg: JPEG library reports unrecoverable error: %s", buffer);

	jmpbufw = (jmpbuf_wrapper *)cinfo->client_data;
	jpeg_destroy(cinfo);

	if(jmpbufw != 0) {
		longjmp(jmpbufw->jmpbuf, 1);
		gd_error_ex(GD_ERROR, "gd-jpeg: EXTREMELY fatal error: longjmp returned control; terminating\n");
	} else {
		gd_error_ex(GD_ERROR, "gd-jpeg: EXTREMELY fatal error: jmpbuf unrecoverable; terminating\n");
	}

	exit(99);
}

static int _gdImageJpegCtx(gdImagePtr im, gdIOCtx *outfile, int quality);

/*
 * Write IM to OUTFILE as a JFIF-formatted JPEG image, using quality
 * QUALITY.  If QUALITY is in the range 0-100, increasing values
 * represent higher quality but also larger image size.  If QUALITY is
 * negative, the IJG JPEG library's default quality is used (which
 * should be near optimal for many applications).  See the IJG JPEG
 * library documentation for more details.
 */


/*
  Function: gdImageJpeg

    <gdImageJpeg> outputs the specified image to the specified file in
    JPEG format. The file must be open for writing. Under MSDOS and
    all versions of Windows, it is important to use "wb" as opposed to
    simply "w" as the mode when opening the file, and under Unix there
    is no penalty for doing so. <gdImageJpeg> does not close the file;
    your code must do so.

    If _quality_ is negative, the default IJG JPEG quality value (which
    should yield a good general quality / size tradeoff for most
    situations) is used. Otherwise, for practical purposes, _quality_
    should be a value in the range 0-95, higher quality values usually
    implying both higher quality and larger image sizes.

    If you have set image interlacing using <gdImageInterlace>, this
    function will interpret that to mean you wish to output a
    progressive JPEG. Some programs (e.g., Web browsers) can display
    progressive JPEGs incrementally; this can be useful when browsing
    over a relatively slow communications link, for
    example. Progressive JPEGs can also be slightly smaller than
    sequential (non-progressive) JPEGs.

  Variants:

    <gdImageJpegCtx> stores the image using a <gdIOCtx> struct.

    <gdImageJpegPtr> stores the image to RAM.

  Parameters:

    im      - The image to save
    outFile - The FILE pointer to write to.
    quality - Compression quality (0-95, 0 means use the default).

  Returns:

    Nothing.

  Example:
    (start code)

    gdImagePtr im;
    int black, white;
    FILE *out;
    // Create the image
    im = gdImageCreate(100, 100);
    // Allocate background
    white = gdImageColorAllocate(im, 255, 255, 255);
    // Allocate drawing color
    black = gdImageColorAllocate(im, 0, 0, 0);
    // Draw rectangle
    gdImageRectangle(im, 0, 0, 99, 99, black);
    // Open output file in binary mode
    out = fopen("rect.jpg", "wb");
    // Write JPEG using default quality
    gdImageJpeg(im, out, -1);
    // Close file
    fclose(out);
    // Destroy image
    gdImageDestroy(im);

    (end code)
*/

BGD_DECLARE(void) gdImageJpeg(gdImagePtr im, FILE *outFile, int quality)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) return;
	gdImageJpegCtx(im, out, quality);
	out->gd_free(out);
}

/*
  Function: gdImageJpegPtr

    Identical to <gdImageJpeg> except that it returns a pointer to a
    memory area with the JPEG data. This memory must be freed by the
    caller when it is no longer needed.

    The caller *must* invoke <gdFree>, not free().  This is because it
    is not guaranteed that libgd will use the same implementation of
    malloc, free, etc. as your proggram.

    The 'size' parameter receives the total size of the block of
    memory.

  Parameters:

    im      - The image to write
    size    - Output: the size of the resulting image.
    quality - Compression quality.

  Returns:

    A pointer to the JPEG data or NULL if an error occurred.

*/
BGD_DECLARE(void *) gdImageJpegPtr(gdImagePtr im, int *size, int quality)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	if (out == NULL) return NULL;
	if (!_gdImageJpegCtx(im, out, quality)) {
		rv = gdDPExtractData(out, size);
	} else {
		rv = NULL;
	}
	out->gd_free(out);
	return rv;
}

void jpeg_gdIOCtx_dest(j_compress_ptr cinfo, gdIOCtx *outfile);

/*
  Function: gdImageJpegCtx

    Write the image as JPEG data via a <gdIOCtx>. See <gdImageJpeg>
    for more details.

  Parameters:

    im      - The image to write.
    outfile - The output sink.
    quality - Image quality.

*/
BGD_DECLARE(void) gdImageJpegCtx(gdImagePtr im, gdIOCtx *outfile, int quality)
{
	_gdImageJpegCtx(im, outfile, quality);
}

/* returns 0 on success, 1 on failure */
static int _gdImageJpegCtx(gdImagePtr im, gdIOCtx *outfile, int quality)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int i, j, jidx;
	/* volatile so we can gdFree it on return from longjmp */
	volatile JSAMPROW row = 0;
	JSAMPROW rowptr[1];
	jmpbuf_wrapper jmpbufw;
	JDIMENSION nlines;
	char comment[255];

#ifdef JPEG_DEBUG
	gd_error_ex(GD_DEBUG, "gd-jpeg: gd JPEG version %s\n", GD_JPEG_VERSION);
	gd_error_ex(GD_DEBUG, "gd-jpeg: JPEG library version %d, %d-bit sample values\n", JPEG_LIB_VERSION, BITS_IN_JSAMPLE);
	if (!im->trueColor) {
		for(i = 0; i < im->colorsTotal; i++) {
			if(!im->open[i]) {
				gd_error_ex(GD_DEBUG, "gd-jpeg: gd colormap index %d: (%d, %d, %d)\n", i, im->red[i], im->green[i], im->blue[i]);
			}
		}
	}
#endif /* JPEG_DEBUG */

	memset(&cinfo, 0, sizeof(cinfo));
	memset(&jerr, 0, sizeof(jerr));

	cinfo.err = jpeg_std_error(&jerr);
	cinfo.client_data = &jmpbufw;

	if(setjmp(jmpbufw.jmpbuf) != 0) {
		/* we're here courtesy of longjmp */
		if(row) {
			gdFree(row);
		}
		return 1;
	}

	cinfo.err->emit_message = jpeg_emit_message;
	cinfo.err->error_exit = fatal_jpeg_error;

	jpeg_create_compress(&cinfo);

	cinfo.image_width = im->sx;
	cinfo.image_height = im->sy;
	cinfo.input_components = 3; /* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; /* colorspace of input image */

	jpeg_set_defaults(&cinfo);

	cinfo.density_unit = 1;
	cinfo.X_density = im->res_x;
	cinfo.Y_density = im->res_y;

	if(quality >= 0) {
		jpeg_set_quality(&cinfo, quality, TRUE);
		if (quality >= 90) {
			cinfo.comp_info[0].h_samp_factor = 1;
			cinfo.comp_info[0].v_samp_factor = 1;
		}
	}

	/* If user requests interlace, translate that to progressive JPEG */
	if(gdImageGetInterlaced(im)) {
#ifdef JPEG_DEBUG
		gd_error_ex(GD_DEBUG, "gd-jpeg: interlace set, outputting progressive JPEG image\n");
#endif
		jpeg_simple_progression(&cinfo);
	}

	jpeg_gdIOCtx_dest(&cinfo, outfile);

	row = (JSAMPROW)gdCalloc(1, cinfo.image_width * cinfo.input_components * sizeof(JSAMPLE));
	if(row == 0) {
		gd_error("gd-jpeg: error: unable to allocate JPEG row structure: gdCalloc returns NULL\n");
		jpeg_destroy_compress(&cinfo);
		return 1;
	}

	rowptr[0] = row;

	jpeg_start_compress(&cinfo, TRUE);

	sprintf(comment, "CREATOR: gd-jpeg v%s (using IJG JPEG v%d),", GD_JPEG_VERSION, JPEG_LIB_VERSION);

	if(quality >= 0) {
		sprintf (comment + strlen(comment), " quality = %d\n", quality);
	} else {
		strcat(comment + strlen(comment), " default quality\n");
	}

	jpeg_write_marker(&cinfo, JPEG_COM, (unsigned char *) comment, (unsigned int)strlen(comment));

	if(im->trueColor) {
#if BITS_IN_JSAMPLE == 12
		gd_error(
		        "gd-jpeg: error: jpeg library was compiled for 12-bit\n"
		        "precision. This is mostly useless, because JPEGs on the web are\n"
		        "8-bit and such versions of the jpeg library won't read or write\n"
		        "them. GD doesn't support these unusual images. Edit your\n"
		        "jmorecfg.h file to specify the correct precision and completely\n"
		        "'make clean' and 'make install' libjpeg again. Sorry.\n"
		       );
		goto error;
#endif /* BITS_IN_JSAMPLE == 12 */
		for(i = 0; i < im->sy; i++) {
			for(jidx = 0, j = 0; j < im->sx; j++) {
				int val = im->tpixels[i][j];
				row[jidx++] = gdTrueColorGetRed(val);
				row[jidx++] = gdTrueColorGetGreen(val);
				row[jidx++] = gdTrueColorGetBlue(val);
			}

			nlines = jpeg_write_scanlines(&cinfo, rowptr, 1);

			if(nlines != 1) {
				gd_error("gd_jpeg: warning: jpeg_write_scanlines returns %u -- expected 1\n", nlines);
			}
		}
	} else {
		for(i = 0; i < im->sy; i++) {
			for(jidx = 0, j = 0; j < im->sx; j++) {
				int idx = im->pixels[i][j];

				/*
				 * NB: Although gd RGB values are ints, their max value is
				 * 255 (see the documentation for gdImageColorAllocate())
				 * -- perfect for 8-bit JPEG encoding (which is the norm)
				 */
#if BITS_IN_JSAMPLE == 8
				row[jidx++] = im->red[idx];
				row[jidx++] = im->green[idx];
				row[jidx++] = im->blue[idx];
#elif BITS_IN_JSAMPLE == 12
				row[jidx++] = im->red[idx] << 4;
				row[jidx++] = im->green[idx] << 4;
				row[jidx++] = im->blue[idx] << 4;
#else
#error IJG JPEG library BITS_IN_JSAMPLE value must be 8 or 12
#endif
			}

			nlines = jpeg_write_scanlines(&cinfo, rowptr, 1);
			if(nlines != 1) {
				gd_error("gd_jpeg: warning: jpeg_write_scanlines"
				         " returns %u -- expected 1\n", nlines);
			}
		}
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	gdFree(row);
	return 0;
}




/*
  Function: gdImageCreateFromJpeg

  See <gdImageCreateFromJpegEx>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpeg(FILE *inFile)
{
	return gdImageCreateFromJpegEx(inFile, 1);
}


/*
  Function: gdImageCreateFromJpegEx

    <gdImageCreateFromJpegEx> is called to load truecolor images from
    JPEG format files. Invoke <gdImageCreateFromJpegEx> with an
    already opened pointer to a file containing the desired
    image. <gdImageCreateFromJpegEx> returns a <gdImagePtr> to the new
    truecolor image, or NULL if unable to load the image (most often
    because the file is corrupt or does not contain a JPEG
    image). <gdImageCreateFromJpegEx> does not close the file.

    You can inspect the sx and sy members of the image to determine
    its size. The image must eventually be destroyed using
    <gdImageDestroy>.

    *The returned image is always a truecolor image.*

  Variants:

    <gdImageCreateFromJpegPtrEx> creates an image from JPEG data
    already in memory.

    <gdImageCreateFromJpegCtxEx> reads its data via the function
    pointers in a <gdIOCtx> structure.

    <gdImageCreateFromJpeg>, <gdImageCreateFromJpegPtr> and
    <gdImageCreateFromJpegCtx> are equivalent to calling their
    _Ex_-named counterparts with an ignore_warning set to 1
    (i.e. TRUE).

  Parameters:

    infile          - The input FILE pointer.
    ignore_warning  - Flag.  If true, ignores recoverable warnings.

  Returns:

    A pointer to the new *truecolor* image.  This will need to be
    destroyed with <gdImageDestroy> once it is no longer needed.

    On error, returns NULL.

  Example:
    (start code)

    gdImagePtr im;
    FILE *in;
    in = fopen("myjpeg.jpg", "rb");
    im = gdImageCreateFromJpegEx(in, GD_TRUE);
    fclose(in);
    // ... Use the image ...
    gdImageDestroy(im);

    (end code)
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegEx(FILE *inFile, int ignore_warning)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx(inFile);
	if (in == NULL) return NULL;
	im = gdImageCreateFromJpegCtxEx(in, ignore_warning);
	in->gd_free(in);
	return im;
}

/*
  Function: gdImageCreateFromJpegPtr

  Parameters:

    size    - size of JPEG data in bytes.
    data    - pointer to JPEG data.

  See <gdImageCreateFromJpegEx>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegPtr(int size, void *data)
{
	return gdImageCreateFromJpegPtrEx(size, data, 1);
}

/*
  Function: gdImageCreateFromJpegPtrEx

  Parameters:

    size            - size of JPEG data in bytes.
    data            - pointer to JPEG data.
    ignore_warning  - if true, ignore recoverable warnings

  See <gdImageCreateFromJpegEx>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegPtrEx(int size, void *data, int ignore_warning)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	if(!in) {
		return 0;
	}
	im = gdImageCreateFromJpegCtxEx(in, ignore_warning);
	in->gd_free(in);
	return im;
}

void jpeg_gdIOCtx_src(j_decompress_ptr cinfo, gdIOCtx *infile);

static int CMYKToRGB(int c, int m, int y, int k, int inverted);

/*
  Function: gdImageCreateFromJpegCtx

  See <gdImageCreateFromJpeg>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegCtx(gdIOCtx *infile)
{
	return gdImageCreateFromJpegCtxEx(infile, 1);
}

/*
  Function: gdImageCreateFromJpegCtxEx

  See <gdImageCreateFromJpeg>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegCtxEx(gdIOCtx *infile, int ignore_warning)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	jmpbuf_wrapper jmpbufw;
	/* volatile so we can gdFree them after longjmp */
	volatile JSAMPROW row = 0;
	volatile gdImagePtr im = 0;
	JSAMPROW rowptr[1];
	JDIMENSION i, j;
	int retval;
	JDIMENSION nrows;
	int channels = 3;
	int inverted = 0;

#ifdef JPEG_DEBUG
	gd_error_ex(GD_DEBUG, "gd-jpeg: gd JPEG version %s\n", GD_JPEG_VERSION);
	gd_error_ex(GD_DEBUG, "gd-jpeg: JPEG library version %d, %d-bit sample values\n", JPEG_LIB_VERSION, BITS_IN_JSAMPLE);
	gd_error_ex(GD_DEBUG, "sizeof: %d\n", sizeof(struct jpeg_decompress_struct));
#endif

	memset(&cinfo, 0, sizeof(cinfo));
	memset(&jerr, 0, sizeof(jerr));

	jmpbufw.ignore_warning = ignore_warning;

	cinfo.err = jpeg_std_error(&jerr);
	cinfo.client_data = &jmpbufw;

	cinfo.err->emit_message = jpeg_emit_message;

	if(setjmp(jmpbufw.jmpbuf) != 0) {
		/* we're here courtesy of longjmp */
		if(row) {
			gdFree(row);
		}
		if(im) {
			gdImageDestroy(im);
		}
		return 0;
	}

	cinfo.err->error_exit = fatal_jpeg_error;

	jpeg_create_decompress(&cinfo);

	jpeg_gdIOCtx_src(&cinfo, infile);

	/* 2.0.22: save the APP14 marker to check for Adobe Photoshop CMYK
	 * files with inverted components.
	 */
	jpeg_save_markers(&cinfo, JPEG_APP0 + 14, 256);

	retval = jpeg_read_header(&cinfo, TRUE);
	if(retval != JPEG_HEADER_OK) {
		gd_error("gd-jpeg: warning: jpeg_read_header returns"
		         " %d, expected %d\n", retval, JPEG_HEADER_OK);
	}

	if(cinfo.image_height > INT_MAX) {
		gd_error("gd-jpeg: warning: JPEG image height (%u) is"
		         " greater than INT_MAX (%d) (and thus greater than"
		         " gd can handle)", cinfo.image_height, INT_MAX);
	}

	if(cinfo.image_width > INT_MAX) {
		gd_error("gd-jpeg: warning: JPEG image width (%u) is"
		         " greater than INT_MAX (%d) (and thus greater than"
		         " gd can handle)\n", cinfo.image_width, INT_MAX);
	}

	im = gdImageCreateTrueColor((int)cinfo.image_width, (int)cinfo.image_height);
	if(im == 0) {
		gd_error("gd-jpeg error: cannot allocate gdImage struct\n");
		goto error;
	}

	/* check if the resolution is specified */
	switch (cinfo.density_unit) {
	case 1:
		im->res_x = cinfo.X_density;
		im->res_y = cinfo.Y_density;
		break;
	case 2:
		im->res_x = DPCM2DPI(cinfo.X_density);
		im->res_y = DPCM2DPI(cinfo.Y_density);
		break;
	}

	/* 2.0.22: very basic support for reading CMYK colorspace files. Nice for
	 * thumbnails but there's no support for fussy adjustment of the
	 * assumed properties of inks and paper.
	 */
	if((cinfo.jpeg_color_space == JCS_CMYK) || (cinfo.jpeg_color_space == JCS_YCCK)) {
		cinfo.out_color_space = JCS_CMYK;
	} else {
		cinfo.out_color_space = JCS_RGB;
	}

	if(jpeg_start_decompress(&cinfo) != TRUE) {
		gd_error("gd-jpeg: warning: jpeg_start_decompress"
		        " reports suspended data source\n");
	}

#ifdef JPEG_DEBUG
	gd_error_ex(GD_DEBUG, "gd-jpeg: JPEG image information:");
	if(cinfo.saw_JFIF_marker) {
		gd_error_ex(GD_DEBUG, " JFIF version %d.%.2d", (int)cinfo.JFIF_major_version, (int)cinfo.JFIF_minor_version);
	} else if(cinfo.saw_Adobe_marker) {
		gd_error_ex(GD_DEBUG, " Adobe format");
	} else {
		gd_error_ex(GD_DEBUG, " UNKNOWN format");
	}

	gd_error_ex(GD_DEBUG, " %ux%u (raw) / %ux%u (scaled) %d-bit", cinfo.image_width,
		    cinfo.image_height, cinfo.output_width,
		    cinfo.output_height, cinfo.data_precision
		);
	gd_error_ex(GD_DEBUG, " %s", (cinfo.progressive_mode ? "progressive" : "baseline"));
	gd_error_ex(GD_DEBUG, " image, %d quantized colors, ", cinfo.actual_number_of_colors);

	switch(cinfo.jpeg_color_space) {
	case JCS_GRAYSCALE:
		gd_error_ex(GD_DEBUG, "grayscale");
		break;

	case JCS_RGB:
		gd_error_ex(GD_DEBUG, "RGB");
		break;

	case JCS_YCbCr:
		gd_error_ex(GD_DEBUG, "YCbCr (a.k.a. YUV)");
		break;

	case JCS_CMYK:
		gd_error_ex(GD_DEBUG, "CMYK");
		break;

	case JCS_YCCK:
		gd_error_ex(GD_DEBUG, "YCbCrK");
		break;

	default:
		gd_error_ex(GD_DEBUG, "UNKNOWN (value: %d)", (int)cinfo.jpeg_color_space);
		break;
	}

	gd_error_ex(GD_DEBUG, " colorspace\n");
	fflush(stdout);
#endif /* JPEG_DEBUG */

	/* REMOVED by TBB 2/12/01. This field of the structure is
	 * documented as private, and sure enough it's gone in the
	 * latest libjpeg, replaced by something else. Unfortunately
	 * there is still no right way to find out if the file was
	 * progressive or not; just declare your intent before you
	 * write one by calling gdImageInterlace(im, 1) yourself.
	 * After all, we're not really supposed to rework JPEGs and
	 * write them out again anyway. Lossy compression, remember? */
#if 0
	gdImageInterlace (im, cinfo.progressive_mode != 0);
#endif
	if(cinfo.out_color_space == JCS_RGB) {
		if(cinfo.output_components != 3) {
			gd_error("gd-jpeg: error: JPEG color quantization"
			         " request resulted in output_components == %d"
			         " (expected 3 for RGB)\n", cinfo.output_components);
			goto error;
		}
		channels = 3;
	} else if(cinfo.out_color_space == JCS_CMYK) {
		jpeg_saved_marker_ptr marker;
		if(cinfo.output_components != 4) {
			gd_error("gd-jpeg: error: JPEG color quantization"
			         " request resulted in output_components == %d"
			         " (expected 4 for CMYK)\n", cinfo.output_components);
			goto error;
		}
		channels = 4;

		marker = cinfo.marker_list;
		while(marker) {
			if(	(marker->marker == (JPEG_APP0 + 14)) &&
			        (marker->data_length >= 12) &&
			        (!strncmp((const char *)marker->data, "Adobe", 5))) {
				inverted = 1;
				break;
			}
			marker = marker->next;
		}
	} else {
		gd_error("gd-jpeg: error: unexpected colorspace\n");
		goto error;
	}
#if BITS_IN_JSAMPLE == 12
	gd_error_ex(GD_ERROR,
		    "gd-jpeg: error: jpeg library was compiled for 12-bit\n"
		    "precision. This is mostly useless, because JPEGs on the web are\n"
		    "8-bit and such versions of the jpeg library won't read or write\n"
		    "them. GD doesn't support these unusual images. Edit your\n"
		    "jmorecfg.h file to specify the correct precision and completely\n"
		    "'make clean' and 'make install' libjpeg again. Sorry.\n");
	goto error;
#endif /* BITS_IN_JSAMPLE == 12 */

	row = gdCalloc(cinfo.output_width *channels, sizeof(JSAMPLE));
	if(row == 0) {
		gd_error("gd-jpeg: error: unable to allocate row for"
		         " JPEG scanline: gdCalloc returns NULL\n");
		goto error;
	}
	rowptr[0] = row;
	if(cinfo.out_color_space == JCS_CMYK) {
		for(i = 0; i < cinfo.output_height; i++) {
			register JSAMPROW currow = row;
			register int *tpix = im->tpixels[i];
			nrows = jpeg_read_scanlines(&cinfo, rowptr, 1);
			if(nrows != 1) {
				gd_error("gd-jpeg: error: jpeg_read_scanlines"
				         " returns %u, expected 1\n", nrows);
				goto error;
			}
			for(j = 0; j < cinfo.output_width; j++, currow += 4, tpix++) {
				*tpix = CMYKToRGB(currow[0], currow[1], currow[2], currow[3], inverted);
			}
		}
	} else {
		for(i = 0; i < cinfo.output_height; i++) {
			register JSAMPROW currow = row;
			register int *tpix = im->tpixels[i];
			nrows = jpeg_read_scanlines(&cinfo, rowptr, 1);
			if(nrows != 1) {
				gd_error("gd-jpeg: error: jpeg_read_scanlines"
				         " returns %u, expected 1\n", nrows);
				goto error;
			}
			for(j = 0; j < cinfo.output_width; j++, currow += 3, tpix++) {
				*tpix = gdTrueColor(currow[0], currow[1], currow[2]);
			}
		}
	}

	if(jpeg_finish_decompress (&cinfo) != TRUE) {
		gd_error("gd-jpeg: warning: jpeg_finish_decompress"
		         " reports suspended data source\n");
	}
	/* TBB 2.0.29: we should do our best to read whatever we can read, and a
	 * warning is a warning. A fatal error on warnings doesn't make sense. */
#if 0
	/* This was originally added by Truxton Fulton */
	if (cinfo.err->num_warnings > 0)
		goto error;
#endif

	jpeg_destroy_decompress(&cinfo);
	gdFree(row);
	return im;

error:
	jpeg_destroy_decompress(&cinfo);

	if(row) {
		gdFree(row);
	}
	if(im) {
		gdImageDestroy(im);
	}

	return 0;
}

/* A very basic conversion approach, TBB */

static int CMYKToRGB(int c, int m, int y, int k, int inverted)
{
	if(inverted) {
		c = 255 - c;
		m = 255 - m;
		y = 255 - y;
		k = 255 - k;
	}

	return gdTrueColor(
	           (255 - c) * (255 - k) / 255,
	           (255 - m) * (255 - k) / 255,
	           (255 - y) * (255 - k) / 255
	       );
#if 0
	if (inverted) {
		c = 255 - c;
		m = 255 - m;
		y = 255 - y;
		k = 255 - k;
	}
	c = c * (255 - k) / 255 + k;
	if (c > 255) {
		c = 255;
	}
	if (c < 0) {
		c = 0;
	}
	m = m * (255 - k) / 255 + k;
	if (m > 255) {
		m = 255;
	}
	if (m < 0) {
		m = 0;
	}
	y = y * (255 - k) / 255 + k;
	if (y > 255) {
		y = 255;
	}
	if (y < 0) {
		y = 0;
	}
	c = 255 - c;
	m = 255 - m;
	y = 255 - y;
	return gdTrueColor (c, m, y);
#endif
}

/*
 * gdIOCtx JPEG data sources and sinks, T. Boutell
 * almost a simple global replace from T. Lane's stdio versions.
 */

/* Expanded data source object for gdIOCtx input */
typedef struct {
	struct jpeg_source_mgr pub;	/* public fields */
	gdIOCtx *infile;			/* source stream */
	unsigned char *buffer;		/* start of buffer */
	boolean start_of_file;	/* have we gotten any data yet? */
}
my_source_mgr;

typedef my_source_mgr *my_src_ptr;

#define INPUT_BUF_SIZE	4096 /* choose an efficiently fread'able size */

/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */

void init_source(j_decompress_ptr cinfo)
{
	my_src_ptr src = (my_src_ptr)cinfo->src;

	/* We reset the empty-input-file flag for each image,
	 * but we don't clear the input buffer.
	 * This is correct behavior for reading a series of images from one source.
	 */
	src->start_of_file = TRUE;
}


/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 * In typical applications, this should read fresh data into the buffer
 * (ignoring the current state of next_input_byte & bytes_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been reloaded.  It is not necessary to
 * fill the buffer entirely, only to obtain at least one more byte.
 *
 * There is no such thing as an EOF return.  If the end of the file has been
 * reached, the routine has a choice of ERREXIT() or inserting fake data into
 * the buffer.  In most cases, generating a warning message and inserting a
 * fake EOI marker is the best course of action --- this will allow the
 * decompressor to output however much of the image is there.  However,
 * the resulting error message is misleading if the real problem is an empty
 * input file, so we handle that case specially.
 *
 * In applications that need to be able to suspend compression due to input
 * not being available yet, a FALSE return indicates that no more data can be
 * obtained right now, but more may be forthcoming later.  In this situation,
 * the decompressor will return to its caller (with an indication of the
 * number of scanlines it has read, if any).  The application should resume
 * decompression after it has loaded more data into the input buffer.  Note
 * that there are substantial restrictions on the use of suspension --- see
 * the documentation.
 *
 * When suspending, the decompressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_input_byte & bytes_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point must be rescanned after resumption, so move it to
 * the front of the buffer rather than discarding it.
 */

#define END_JPEG_SEQUENCE "\r\n[*]--:END JPEG:--[*]\r\n"

boolean fill_input_buffer(j_decompress_ptr cinfo)
{
	my_src_ptr src = (my_src_ptr)cinfo->src;
	/* 2.0.12: signed size. Thanks to Geert Jansen */
	/* 2.0.14: some platforms (mingw-msys) don't have ssize_t. Call
	 * an int an int.
	 */
	int nbytes = 0;
	memset(src->buffer, 0, INPUT_BUF_SIZE);

	while(nbytes < INPUT_BUF_SIZE) {
		int got = gdGetBuf(src->buffer + nbytes, INPUT_BUF_SIZE - nbytes, src->infile);

		if((got == EOF) || (got == 0)) {
			/* EOF or error. If we got any data, don't worry about it.
			 * If we didn't, then this is unexpected. */
			if(!nbytes) {
				nbytes = -1;
			}
			break;
		}
		nbytes += got;
	}

	if(nbytes <= 0) {
		if(src->start_of_file) {
			/* Treat empty input file as fatal error */
			ERREXIT(cinfo, JERR_INPUT_EMPTY);
		}
		WARNMS(cinfo, JWRN_JPEG_EOF);
		/* Insert a fake EOI marker */
		src->buffer[0] = (unsigned char)0xFF;
		src->buffer[1] = (unsigned char)JPEG_EOI;
		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = FALSE;

	return TRUE;
}

/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 * Writers of suspendable-input applications must note that skip_input_data
 * is not granted the right to give a suspension return.  If the skip extends
 * beyond the data currently in the buffer, the buffer can be marked empty so
 * that the next read will cause a fill_input_buffer call that can suspend.
 * Arranging for additional bytes to be discarded before reloading the input
 * buffer is the application writer's problem.
 */

void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	my_src_ptr src = (my_src_ptr)cinfo->src;

	/* Just a dumb implementation for now. Not clear that being smart is worth
	 * any trouble anyway --- large skips are infrequent.
	 */
	if(num_bytes > 0) {
		while(num_bytes > (long)src->pub.bytes_in_buffer) {
			num_bytes -= (long)src->pub.bytes_in_buffer;
			(void)fill_input_buffer(cinfo);
			/* note we assume that fill_input_buffer will never return FALSE,
			 * so suspension need not be handled.
			 */
		}
		src->pub.next_input_byte += (size_t)num_bytes;
		src->pub.bytes_in_buffer -= (size_t)num_bytes;
	}
}

/*
 * An additional method that can be provided by data source modules is the
 * resync_to_restart method for error recovery in the presence of RST markers.
 * For the moment, this source module just uses the default resync method
 * provided by the JPEG library.  That method assumes that no backtracking
 * is possible.
 */

/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */
void term_source(j_decompress_ptr cinfo)
{
	(void)cinfo;
}


/*
 * Prepare for input from a gdIOCtx stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing decompression.
 */

void jpeg_gdIOCtx_src(j_decompress_ptr cinfo, gdIOCtx *infile)
{
	my_src_ptr src;

	/* The source object and input buffer are made permanent so that a series
	 * of JPEG images can be read from the same file by calling jpeg_gdIOCtx_src
	 * only before the first one.  (If we discarded the buffer at the end of
	 * one image, we'd likely lose the start of the next one.)
	 * This makes it unsafe to use this manager and a different source
	 * manager serially with the same JPEG object.  Caveat programmer.
	 */
	if(cinfo->src == NULL) {
		/* first time for this JPEG object? */
		cinfo->src = (struct jpeg_source_mgr *)
		             (*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_PERMANENT,
		                                        sizeof(my_source_mgr));
		src = (my_src_ptr)cinfo->src;
		src->buffer = (unsigned char *)
		              (*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_PERMANENT,
		                      INPUT_BUF_SIZE * sizeof(unsigned char));
	}

	src = (my_src_ptr)cinfo->src;
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->pub.term_source = term_source;
	src->infile = infile;
	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */
}

/* Expanded data destination object for stdio output */
typedef struct {
	struct jpeg_destination_mgr pub; /* public fields */
	gdIOCtx *outfile; /* target stream */
	unsigned char *buffer; /* start of buffer */
}
my_destination_mgr;

typedef my_destination_mgr *my_dest_ptr;

#define OUTPUT_BUF_SIZE	4096 /* choose an efficiently fwrite'able size */

/*
 * Initialize destination --- called by jpeg_start_compress
 * before any data is actually written.
 */

void init_destination(j_compress_ptr cinfo)
{
	my_dest_ptr dest = (my_dest_ptr)cinfo->dest;

	/* Allocate the output buffer --- it will be released when done with image */
	dest->buffer = (unsigned char *)
	               (*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_IMAGE,
	                       OUTPUT_BUF_SIZE * sizeof(unsigned char));

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}


/*
 * Empty the output buffer --- called whenever buffer fills up.
 *
 * In typical applications, this should write the entire output buffer
 * (ignoring the current state of next_output_byte & free_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been dumped.
 *
 * In applications that need to be able to suspend compression due to output
 * overrun, a FALSE return indicates that the buffer cannot be emptied now.
 * In this situation, the compressor will return to its caller (possibly with
 * an indication that it has not accepted all the supplied scanlines).  The
 * application should resume compression after it has made more room in the
 * output buffer.  Note that there are substantial restrictions on the use of
 * suspension --- see the documentation.
 *
 * When suspending, the compressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_output_byte & free_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point will be regenerated after resumption, so do not
 * write it out when emptying the buffer externally.
 */

boolean empty_output_buffer(j_compress_ptr cinfo)
{
	my_dest_ptr dest = (my_dest_ptr)cinfo->dest;

	if(gdPutBuf(dest->buffer, OUTPUT_BUF_SIZE, dest->outfile) != (size_t)OUTPUT_BUF_SIZE) {
		ERREXIT(cinfo, JERR_FILE_WRITE);
	}

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

	return TRUE;
}


/*
 * Terminate destination --- called by jpeg_finish_compress
 * after all data has been written.  Usually needs to flush buffer.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

void term_destination(j_compress_ptr cinfo)
{
	my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
	int datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

	/* Write any data remaining in the buffer */
	if(datacount > 0) {
		if(gdPutBuf(dest->buffer, datacount, dest->outfile) != datacount) {
			ERREXIT(cinfo, JERR_FILE_WRITE);
		}
	}
}

/*
 * Prepare for output to a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing compression.
 */

void jpeg_gdIOCtx_dest(j_compress_ptr cinfo, gdIOCtx *outfile)
{
	my_dest_ptr dest;

	/* The destination object is made permanent so that multiple JPEG images
	 * can be written to the same file without re-executing jpeg_stdio_dest.
	 * This makes it dangerous to use this manager and a different destination
	 * manager serially with the same JPEG object, because their private object
	 * sizes may be different.  Caveat programmer.
	 */
	if(cinfo->dest == NULL) {
		/* first time for this JPEG object? */
		cinfo->dest = (struct jpeg_destination_mgr *)
		              (*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_PERMANENT,
		                      sizeof(my_destination_mgr));
	}

	dest = (my_dest_ptr)cinfo->dest;
	dest->pub.init_destination = init_destination;
	dest->pub.empty_output_buffer = empty_output_buffer;
	dest->pub.term_destination = term_destination;
	dest->outfile = outfile;
}

#else /* !HAVE_LIBJPEG */

static void _noJpegError(void)
{
	gd_error("JPEG image support has been disabled\n");
}

BGD_DECLARE(void) gdImageJpeg(gdImagePtr im, FILE *outFile, int quality)
{
	_noJpegError();
}

BGD_DECLARE(void *) gdImageJpegPtr(gdImagePtr im, int *size, int quality)
{
	_noJpegError();
	return NULL;
}

BGD_DECLARE(void) gdImageJpegCtx(gdImagePtr im, gdIOCtx *outfile, int quality)
{
	_noJpegError();
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJpeg(FILE *inFile)
{
	_noJpegError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegEx(FILE *inFile, int ignore_warning)
{
	_noJpegError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegPtr(int size, void *data)
{
	_noJpegError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegPtrEx(int size, void *data, int ignore_warning)
{
	_noJpegError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegCtx(gdIOCtx *infile)
{
	_noJpegError();
	return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegCtxEx(gdIOCtx *infile, int ignore_warning)
{
	_noJpegError();
	return NULL;
}

#endif /* HAVE_LIBJPEG */
