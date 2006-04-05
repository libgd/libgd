#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "png.h"    /* includes zlib.h and setjmp.h */

#define TRUE 1
#define FALSE 0

/*---------------------------------------------------------------------------

    gd_png.c                 Copyright 1999 Greg Roelofs and Thomas Boutell

    The routines in this file, gdImagePng*() and gdImageCreateFromPng*(),
    are drop-in replacements for gdImageGif*() and gdImageCreateFromGif*(),
    except that these functions are noisier in the case of errors (comment
    out all fprintf() statements to disable that).

    Only GIF-like PNG features are currently supported; that is, images must
    either be indexed-color to begin with or they will be converted to it,
    and they can have, at most, a single, fully transparent palette entry or
    color.  (Alpha channels are ignored.)  Since gd images are artificially
    generated, gamma is also ignored, and there is currently no support for
    embedded text annotations (a la GIF comments) in gd.

    Last updated:  19 July 1999

  ---------------------------------------------------------------------------*/

typedef struct _jmpbuf_wrapper {
  jmp_buf jmpbuf;
} jmpbuf_wrapper;

static jmpbuf_wrapper gdPngJmpbufStruct;

static void gdPngErrorHandler(png_structp png_ptr, png_const_charp msg)
{
  jmpbuf_wrapper  *jmpbuf_ptr;

  /* This function, aside from the extra step of retrieving the "error
   * pointer" (below) and the fact that it exists within the application
   * rather than within libpng, is essentially identical to libpng's
   * default error handler.  The second point is critical:  since both
   * setjmp() and longjmp() are called from the same code, they are
   * guaranteed to have compatible notions of how big a jmp_buf is,
   * regardless of whether _BSD_SOURCE or anything else has (or has not)
   * been defined. */

  fprintf(stderr, "gd-png:  fatal libpng error: %s\n", msg);
  fflush(stderr);

  jmpbuf_ptr = png_get_error_ptr(png_ptr);
  if (jmpbuf_ptr == NULL) {         /* we are completely hosed now */
    fprintf(stderr,
      "gd-png:  EXTREMELY fatal error: jmpbuf unrecoverable; terminating.\n");
    fflush(stderr);
    exit(99);
  }

  longjmp(jmpbuf_ptr->jmpbuf, 1);
}


static void gdPngReadData(png_structp png_ptr,
	png_bytep data, png_size_t length)
{
	gdGetBuf(data, length, (gdIOCtx *)
		png_get_io_ptr(png_ptr));
}

static void gdPngWriteData(png_structp png_ptr,
	png_bytep data, png_size_t length)
{
	gdPutBuf(data, length, (gdIOCtx *)
		png_get_io_ptr(png_ptr));
}

static void gdPngFlushData(png_structp png_ptr)
{
}

gdImagePtr gdImageCreateFromPng(FILE *inFile)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx(inFile);
	im = gdImageCreateFromPngCtx(in);
	in->free(in);
	return im;
}


/* This routine is based in part on the Chapter 13 demo code in "PNG: The
 *  Definitive Guide" (http://www.cdrom.com/pub/png/pngbook.html).
 */
gdImagePtr gdImageCreateFromPngCtx(gdIOCtx *infile)
{
    png_byte sig[8];
    png_structp png_ptr;
    png_infop info_ptr;
    png_uint_32 width, height, rowbytes;
    int bit_depth, color_type, interlace_type;
    int num_palette, num_trans;
    png_colorp palette;
    png_color_16p trans_gray_rgb;
    png_bytep trans;
    png_bytep image_data = NULL;
    png_bytepp row_pointers = NULL;
    gdImagePtr im = NULL;
    int i, j, *open;
    volatile int transparent = -1;
    volatile int palette_allocated = FALSE;

    /* Make sure the signature can't match by dumb luck -- TBB */
    /* GRR: isn't sizeof(infile) equal to the size of the pointer? */
    memset(infile, 0, sizeof(infile));

    /* first do a quick check that the file really is a PNG image; could
     * have used slightly more general png_sig_cmp() function instead */
    gdGetBuf(sig, 8, infile);
    if (!png_check_sig(sig, 8))
        return NULL;   /* bad signature */

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, &gdPngJmpbufStruct,
      gdPngErrorHandler, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "gd-png error: cannot allocate libpng main struct\n");
        return NULL;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "gd-png error: cannot allocate libpng info struct\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return NULL;
    }

    /* we could create a second info struct here (end_info), but it's only
     * useful if we want to keep pre- and post-IDAT chunk info separated
     * (mainly for PNG-aware image editors and converters) */

    /* setjmp() must be called in every non-callback function that calls a
     * PNG-reading libpng function */
    if (setjmp(gdPngJmpbufStruct.jmpbuf)) {
        fprintf(stderr, "gd-png error: setjmp returns error condition\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }

    png_set_sig_bytes(png_ptr, 8);  /* we already read the 8 signature bytes */

    png_set_read_fn(png_ptr, (void *)infile, gdPngReadData);
    png_read_info(png_ptr, info_ptr);  /* read all PNG info up to image data */

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
      &interlace_type, NULL, NULL);

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    else if (bit_depth < 8)
        png_set_packing(png_ptr);   /* expand to 1 byte per pixel */

    if (color_type & PNG_COLOR_MASK_ALPHA) {
         fprintf(stderr, "gd-png warning: alpha channel not supported\n");
         png_set_strip_alpha(png_ptr);
    }

    switch (color_type) {
        case PNG_COLOR_TYPE_PALETTE:
            png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
            if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
                int real_num_trans = 0, idx_first_trans = -1;
                int min_trans = 256, idx_min_trans = -1;

                png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, NULL);
                for (i = 0;  i < num_trans;  ++i) {
                    if (trans[i] < 255) {
                        ++real_num_trans;
                        if (idx_first_trans < 0)
                            idx_first_trans = i;
                        if (trans[i] < min_trans) {
                            min_trans = trans[i];
                            idx_min_trans = i;
                        }
                    }
                }
                if (real_num_trans > 0) {
                    if (real_num_trans > 1 || trans[idx_first_trans] != 0) {
                        fprintf(stderr, "gd-png warning: only single-color, "
                          "100%% transparency supported\n");
                        transparent = idx_min_trans;
                    } else {
                        transparent = idx_first_trans;
                    }
                }
            }
            break;

        case PNG_COLOR_TYPE_GRAY:
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            /* create a fake palette and check for single-shade transparency */
            if ((palette = (png_colorp)malloc(256*sizeof(png_color))) == NULL) {
                fprintf(stderr, "gd-png error: cannot allocate gray palette\n");
                png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
                return NULL;
            }
            palette_allocated = TRUE;
            num_palette = 256;
            for (i = 0;  i < 256;  ++i) {
                palette[i].red = palette[i].green = palette[i].blue = i;
            }
            if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
                png_get_tRNS(png_ptr, info_ptr, NULL, NULL, &trans_gray_rgb);
                if (bit_depth == 16)  /* png_set_strip_16() not yet in effect */
                    transparent = trans_gray_rgb->gray >> 8;
                else
                    transparent = trans_gray_rgb->gray;
                /* Note slight error in 16-bit case:  up to 256 16-bit shades
                 * may get mapped to a single 8-bit shade, and only one of them
                 * is supposed to be transparent.  IOW, both opaque pixels and
                 * transparent pixels will be mapped into the transparent entry.
                 * There is no particularly good way around this in the case
                 * that all 256 8-bit shades are used, but one could write some
                 * custom 16-bit code to handle the case where there are free
                 * palette entries.  This error will be extremely rare in
                 * general, though.  (Quite possibly there is only one such
                 * image in existence.) */
            }
            break;

        case PNG_COLOR_TYPE_RGB:
        case PNG_COLOR_TYPE_RGB_ALPHA:
            /* allocate a palette and check for single-shade transparency */
            if ((palette = (png_colorp)malloc(256*sizeof(png_color))) == NULL) {
                fprintf(stderr, "gd-png error: cannot allocate RGB palette\n");
                png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
                return NULL;
            }
            palette_allocated = TRUE;
            num_palette = 256;
            if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
                png_get_tRNS(png_ptr, info_ptr, NULL, NULL, &trans_gray_rgb);
                if (bit_depth == 16) {   /* png_set_strip_16() not yet active */
                    palette[0].red   = trans_gray_rgb->red   >> 8;
                    palette[0].green = trans_gray_rgb->green >> 8;
                    palette[0].blue  = trans_gray_rgb->blue  >> 8;
                } else {
                    palette[0].red   = trans_gray_rgb->red;
                    palette[0].green = trans_gray_rgb->green;
                    palette[0].blue  = trans_gray_rgb->blue;
                }
                transparent = 0;
                /* Note that the same error exists in the 16-bit RGB case as in
                 * the grayscale case, except that the degeneracy is now 16.8
                 * million to 1 (at a minimum--actually more than that due to
                 * quantization).  Again, this is an extremely rare problem.
                 * Unfortunately, it also affects 8-bit-per-sample RGB images
                 * (quantization), unless libpng is doing something sneaky... */
            } else {
                palette[0].red = palette[0].green = palette[0].blue = 224;
            }

#if 0  /* libpng.txt demo code looks broken--need to check both PLTE and hIST */
            if (png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE)) {
                png_color_16p histogram;

                png_get_hIST(png_ptr, info_ptr, &histogram);
                png_set_dither(png_ptr, palette, num_palette,
                  max_screen_colors, histogram, 1);
            } else
#endif
            {
                int idx, red, green, blue;

#ifdef PALETTE_6x7x6
                /* allocate a 6x7x6 color cube, starting at index 4 */
                idx = 4;
                for (red = 0;  red < 256;  red += 51) {
                    for (i = 0;  i < 7;  ++i) {
                        green = (i * 425) / 10;            /* i.e., 42.5 */
                        for (blue = 0;  blue < 256;  blue += 51) {
                            palette[idx].red = red;
                            palette[idx].green = green;
                            palette[idx].blue = blue;
                            ++idx;
                        }
                    }
                }
                /* fill in remaining entries (1-3) with common gray values */
                palette[1].red = palette[1].green = palette[1].blue = 192;
                palette[2].red = palette[2].green = palette[2].blue = 128;
                palette[3].red = palette[3].green = palette[3].blue = 64;
                /* final argument (full_dither) *must* be 1: */
                png_set_dither(png_ptr, palette, 256, 256, NULL, 1);
#else
                /* allocate a 6x6x6 color cube, starting at index 0 or 1 */
                idx = (transparent < 0)? 0 : 1;
                for (red = 0;  red < 256;  red += 51) {
                    for (green = 0;  green < 256;  green += 51) {
                        for (blue = 0;  blue < 256;  blue += 51) {
                            palette[idx].red = red;
                            palette[idx].green = green;
                            palette[idx].blue = blue;
                            ++idx;
                        }
                    }
                }
                png_set_dither(png_ptr, palette, idx, idx, NULL, 1);
#endif
            }
            break;
    }

    png_read_update_info(png_ptr, info_ptr);

    /* allocate space for the PNG image data */
    rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    if ((image_data = (png_bytep)malloc(rowbytes*height)) == NULL) {
        fprintf(stderr, "gd-png error: cannot allocate image data\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }
    if ((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL) {
        fprintf(stderr, "gd-png error: cannot allocate row pointers\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(image_data);
        return NULL;
    }

    /* set the individual row_pointers to point at the correct offsets */
    for (j = 0;  j < height;  ++j) {
        row_pointers[j] = image_data + j*rowbytes;
    }

    png_read_image(png_ptr, row_pointers);   /* read whole image... */
    png_read_end(png_ptr, NULL);             /* ...done! */

    if ((im = gdImageCreate((int)width, (int)height)) == NULL) {
        fprintf(stderr, "gd-png error: cannot allocate gdImage struct\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(image_data);
        free(row_pointers);
        return NULL;
    }

    im->colorsTotal = num_palette;
    im->transparent = transparent;
    im->interlace = (interlace_type == PNG_INTERLACE_ADAM7);

    /* load the palette and mark all entries "open" (unused) for now */
    open = im->open;
    for (i = 0;  i < num_palette;  ++i) {
        im->red[i]   = palette[i].red;
        im->green[i] = palette[i].green;
        im->blue[i]  = palette[i].blue;
        open[i] = 1;
    }
    for (i = num_palette;  i < gdMaxColors;  ++i) {
        open[i] = 1;
    }

    /* can't nuke structs until done with palette */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    /* could copy data with memcpy(), but also want to check colormap entries */
    for (j = 0;  j < height;  ++j) {
        for (i = 0;  i < width;  ++i) {
            register png_byte idx = row_pointers[j][i];

            im->pixels[j][i] = idx;
            open[idx] = 0;
        }
    }

#ifdef DEBUG
    for (i = num_palette;  i < gdMaxColors;  ++i) {
        if (!open[i]) {
            fprintf(stderr, "gd-png warning: image data references out-of-range"
              " color index (%d)\n", i);
        }
    }
#endif

    if (palette_allocated)
        free(palette);
    free(image_data);
    free(row_pointers);

    return im;
}


void gdImagePng(gdImagePtr im, FILE *outFile)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	gdImagePngCtx(im, out);
	out->free(out);
}

void* gdImagePngPtr(gdImagePtr im, int *size)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
	gdImagePngCtx(im, out);
	rv = gdDPExtractData(out, size);
	out->free(out);
	return rv;
}

/* This routine is based in part on code from Dale Lutz (Safe Software Inc.)
 *  and in part on demo code from Chapter 15 of "PNG: The Definitive Guide"
 *  (http://www.cdrom.com/pub/png/pngbook.html).
 */
void gdImagePngCtx(gdImagePtr im, gdIOCtx *outfile)
{
    int i, j, bit_depth, interlace_type;
    int width = im->sx;
    int height = im->sy;
    int colors = im->colorsTotal;
    int *open = im->open;
    int mapping[gdMaxColors];		/* mapping[gif_index] == png_index */
    png_byte trans_value = 0;
    png_color palette[gdMaxColors];
    png_structp png_ptr;
    png_infop info_ptr;
    volatile int transparent = im->transparent;
    volatile int remap = FALSE;


    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
      &gdPngJmpbufStruct, gdPngErrorHandler, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "gd-png error: cannot allocate libpng main struct\n");
        return;
    }

    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "gd-png error: cannot allocate libpng info struct\n");
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        return;
    }

    if (setjmp(gdPngJmpbufStruct.jmpbuf)) {
        fprintf(stderr, "gd-png error: setjmp returns error condition\n");
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return;
    }

    png_set_write_fn(png_ptr, (void *)outfile, gdPngWriteData, gdPngFlushData);

    /* For now gd only supports palette images, for which filter type NONE is
     * almost guaranteed to be the best.  But that's what libpng defaults to
     * for palette images anyway, so no need to set this explicitly. */
/*  png_set_filter(png_ptr, 0, PNG_FILTER_NONE);  */

    /* may want to force maximum compression, but time penalty is large */
/*  png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);  */

    /* can set this to a smaller value without compromising compression if all
     * image data is 16K or less; will save some decoder memory [min == 8] */
/*  png_set_compression_window_bits(png_ptr, 15);  */

    if (transparent >= im->colorsTotal ||
       (transparent >= 0 && open[transparent])) 
        transparent = -1;

    for (i = 0;  i < gdMaxColors;  ++i)
        mapping[i] = -1;

    /* count actual number of colors used (colorsTotal == high-water mark) */
    colors = 0;
    for (i = 0;  i < im->colorsTotal;  ++i) {
        if (!open[i]) {
            mapping[i] = colors;
            ++colors;
        }
    }
    if (colors < im->colorsTotal) {
        remap = TRUE;
        if (transparent >= 0)
            transparent = mapping[transparent];
    }

    if (colors <= 2)
        bit_depth = 1;
    else if (colors <= 4)
        bit_depth = 2;
    else if (colors <= 16)
        bit_depth = 4;
    else
        bit_depth = 8;

    interlace_type = im->interlace? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

    png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
      PNG_COLOR_TYPE_PALETTE, interlace_type,
      PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    if (transparent >= 0) {
        /* always write PNG files with the transparent palette entry first to
         * minimize size of the tRNS chunk; swap if necessary */
        if (transparent != 0) {
            if (!remap) {		/* so colors == im->colorsTotal */
                remap = TRUE;
                for (i = 0;  i < colors;  ++i)
                    mapping[i] = i;
            }
            mapping[transparent] = 0;
            mapping[0] = transparent;
        }
        png_set_tRNS(png_ptr, info_ptr, &trans_value, 1, NULL);
    }

    /* convert GIF palette to libpng layout */
    if (remap)
        for (i = 0;  i < im->colorsTotal;  ++i) {
            if (mapping[i] < 0)
                continue;
            palette[mapping[i]].red   = im->red[i];
            palette[mapping[i]].green = im->green[i];
            palette[mapping[i]].blue  = im->blue[i];
        }
    else
        for (i = 0;  i < colors;  ++i) {
            palette[i].red   = im->red[i];
            palette[i].green = im->green[i];
            palette[i].blue  = im->blue[i];
        }
    png_set_PLTE(png_ptr, info_ptr, palette, colors);


    /* write out the PNG header info (everything up to first IDAT) */
    png_write_info(png_ptr, info_ptr);

    /* make sure < 8-bit images are packed into pixels as tightly as possible */
    png_set_packing(png_ptr);

    /* This code allocates a set of row buffers and copies the gd image data
     * into them only in the case that remapping is necessary; in gd 1.3 and
     * later, the im->pixels array is laid out identically to libpng's row
     * pointers and can be passed to png_write_image() function directly.
     * The remapping case could be accomplished with less memory for non-
     * interlaced images, but interlacing causes some serious complications. */
    if (remap) {
        png_bytep *row_pointers;
	row_pointers = malloc(sizeof(png_bytep) * height);
        if (row_pointers == NULL) {
            fprintf(stderr, "gd-png error: unable to allocate row_pointers\n");
        }
        for (j = 0;  j < height;  ++j) {
            if ((row_pointers[j] = (png_bytep)malloc(width)) == NULL) {
                fprintf(stderr, "gd-png error: unable to allocate rows\n");
                for (i = 0;  i < j;  ++i)
                    free(row_pointers[i]);
                return;
            }
            for (i = 0;  i < width;  ++i)
                row_pointers[j][i] = mapping[im->pixels[j][i]];
        }

        png_write_image(png_ptr, row_pointers);
        png_write_end(png_ptr, info_ptr);

        for (j = 0;  j < height;  ++j)
            free(row_pointers[j]);
	free(row_pointers);
    } else {
        png_write_image(png_ptr, im->pixels);
        png_write_end(png_ptr, info_ptr);
    }
    /* 1.6.3: maybe we should give that memory BACK! TBB */
        png_destroy_write_struct(&png_ptr, &info_ptr);
}


