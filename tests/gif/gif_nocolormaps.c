/**
 * Ensure that a GIF without any Global or Local color tables is still decoded
 *
 * GIF89a spec indicates conforming image files need not have Global or Local color tables at all
 *
 * See also:	https://www.w3.org/Graphics/GIF/spec-gif89a.txt
 *		http://probablyprogramming.com/2009/03/15/the-tiniest-gif-ever
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;

	fp = gdTestFileOpen2("gif", "gif_nocolormaps.gif");
	gdTestAssert(fp != NULL);
	im = gdImageCreateFromGif(fp);
	gdTestAssert(im != NULL);
	fclose(fp);
    gdImageDestroy(im);

	return gdNumFailures();
}
