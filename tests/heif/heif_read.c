/**
 * Simple test case that confirms the failure of using `gdImageCreateFromHeif`
 * with a NULL pointer.
 */


#include "gd.h"
#include "gdtest.h"

#include <libheif/heif.h>

int main()
{
	gdImagePtr im;
	FILE *fp;

	if (!gdTestAssertMsg(heif_have_decoder_for_format(heif_compression_HEVC), "HEVC codec support missing from libheif\n"))
		return 77;

	fp = gdTestFileOpen2("heif", "label.heic");
	gdTestAssert(fp != NULL);
	im = gdImageCreateFromHeif(fp);
	if (gdTestAssert(im != NULL))
		gdImageDestroy(im);
	fclose(fp);

	return gdNumFailures();
}
