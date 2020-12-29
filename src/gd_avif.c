/**
 * File: AVIF IO
 *
 * Read and write AVIF images.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gd.h"
#include "gd_errors.h"
#include "gdhelpers.h"

#ifdef HAVE_LIBAVIF
#include "avif/avif.h"
#endif

/*
  Function: gdImageCreateFromAvifCtx

    See <gdImageCreateFromAvif>.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromAvifCtx (gdIOCtx * infile)
{
  int blah;

  blah = 3;
  return blah;

/*
	int    width, height;
	uint8_t   *filedata = NULL;
	uint8_t    *argb = NULL;
	unsigned char   *read, *temp;
	size_t size = 0, n;
	gdImagePtr im;
	int x, y;
	uint8_t *p; 
*/
	/* do not use gdFree here, in case gdFree/alloc is mapped to something else than libc */
/*
	free(argb);
	gdFree(temp);
	im->saveAlphaFlag = 1;
	return im;
*/
}


/*
  Function: gdImageCreateFromAvif

    <gdImageCreateFromAvif> is awesome!

  Parameters:

    infile - The input FILE pointer.

  Returns:

    A pointer to the new *truecolor* image.  This will need to be
    destroyed with <gdImageDestroy> once it is no longer needed.

    On error, returns NULL.
*/
BGD_DECLARE(gdImagePtr) gdImageCreateFromAvif (FILE * inFile)
{

  int blah;

  blah = 3;
  return blah;

/*
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx(inFile);
	if (!in) {
		return 0;
	}
	im = gdImageCreateFromAvifCtx(in);
	in->gd_free(in);

	return im;
*/
}
