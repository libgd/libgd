/* Convenience functions to read or write images from or to disk,
 * determining file type from the filename extension. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gd_intern.h"
#include <stdio.h>
#include <string.h>
typedef gdImagePtr(BGD_STDCALL *ReadFn)(FILE *in);
typedef void(BGD_STDCALL *WriteFn)(gdImagePtr im, FILE *out);
typedef gdImagePtr(BGD_STDCALL *LoadFn)(char *filename);

#ifdef HAVE_LIBZ
static void BGD_STDCALL writegd2(gdImagePtr im, FILE *out)
{
    gdImageGd2(im, out, 0, GD2_FMT_COMPRESSED);
} /* writegd*/
#endif

#ifdef HAVE_LIBJPEG
static void BGD_STDCALL writejpeg(gdImagePtr im, FILE *out)
{
    gdImageJpeg(im, out, -1);
} /* writejpeg*/
#endif

static void BGD_STDCALL writewbmp(gdImagePtr im, FILE *out)
{
    int fg = gdImageColorClosest(im, 0, 0, 0);

    gdImageWBMP(im, fg, out);
} /* writejpeg*/

static void BGD_STDCALL writebmp(gdImagePtr im, FILE *out)
{
    gdImageBmp(im, out, GD_TRUE);
} /* writejpeg*/

static const struct FileType {
    const char *ext;
    ReadFn reader;
    WriteFn writer;
    LoadFn loader;
} Types[] = {{".gif", gdImageCreateFromGif, gdImageGif, NULL},
             {".gd", gdImageCreateFromGd, gdImageGd, NULL},
             {".wbmp", gdImageCreateFromWBMP, writewbmp, NULL},
             {".bmp", gdImageCreateFromBmp, writebmp, NULL},

             {".xbm", gdImageCreateFromXbm, NULL, NULL},
             {".tga", gdImageCreateFromTga, NULL, NULL},

#ifdef HAVE_LIBAVIF
             {".avif", gdImageCreateFromAvif, gdImageAvif, NULL},
#endif

#ifdef HAVE_LIBPNG
             {".png", gdImageCreateFromPng, gdImagePng, NULL},
#endif

             {".qoi", gdImageCreateFromQoi, gdImageQoi, NULL},

#ifdef HAVE_LIBJPEG
             {".jpg", gdImageCreateFromJpeg, writejpeg, NULL},
             {".jpeg", gdImageCreateFromJpeg, writejpeg, NULL},
#endif

#ifdef HAVE_LIBHEIF
             {".heic", gdImageCreateFromHeif, gdImageHeif, NULL},
             {".heix", gdImageCreateFromHeif, NULL, NULL},
#endif

#ifdef HAVE_LIBTIFF
             {".tiff", gdImageCreateFromTiff, gdImageTiff, NULL},
             {".tif", gdImageCreateFromTiff, gdImageTiff, NULL},
#endif

#ifdef HAVE_LIBZ
             {".gd2", gdImageCreateFromGd2, writegd2, NULL},
#endif

#ifdef HAVE_LIBWEBP
             {".webp", gdImageCreateFromWebp, gdImageWebp, NULL},
#endif

#ifdef HAVE_LIBJXL
             {".jxl", gdImageCreateFromJxl, gdImageJxl, NULL},
#endif

#ifdef HAVE_LIBXPM
             {".xpm", NULL, NULL, gdImageCreateFromXpm},
#endif

             {NULL, NULL, NULL, NULL}};

static const struct FileType *ftype(const char *filename)
{
    int n;
    const char *ext;

    /* Find the file extension (i.e. the last period in the string. */
    ext = strrchr(filename, '.');
    if (!ext)
        return NULL;

    for (n = 0; Types[n].ext; n++) {
        if (gd_strcasecmp(ext, Types[n].ext) == 0) {
            return &Types[n];
        } /* if */
    } /* for */

    return NULL;
} /* ftype*/

BGD_DECLARE(int)
gdSupportsFileType(const char *filename, int writing)
{
    const struct FileType *entry = ftype(filename);
    return !!entry && (!writing || !!entry->writer);
} /* gdSupportsFileType*/

BGD_DECLARE(gdImagePtr)
gdImageCreateFromFile(const char *filename)
{
    const struct FileType *entry = ftype(filename);
    FILE *fh;
    gdImagePtr result;

    if (!entry)
        return NULL;
    if (entry->loader)
        return entry->loader((char *)filename);
    if (!entry->reader)
        return NULL;

    fh = fopen(filename, "rb");
    if (!fh)
        return NULL;

    result = entry->reader(fh);

    fclose(fh);

    return result;
} /* gdImageCreateFromFile*/

BGD_DECLARE(int)
gdImageFile(gdImagePtr im, const char *filename)
{
    const struct FileType *entry = ftype(filename);
    FILE *fh;

    if (!entry || !entry->writer)
        return GD_FALSE;

    fh = fopen(filename, "wb");
    if (!fh)
        return GD_FALSE;

    entry->writer(im, fh);

    if (fflush(fh) != 0 || ferror(fh)) {
        fclose(fh);
        return GD_FALSE;
    }

    if (fclose(fh) != 0) {
        return GD_FALSE;
    }

    return GD_TRUE;
} /* gdImageFile*/
