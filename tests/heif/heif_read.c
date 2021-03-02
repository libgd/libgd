/**
 * Simple test case that confirms the failure of using `gdImageCreateFromHeif`
 * with a NULL pointer.
 */


#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;

	fp = gdTestFileOpen2("heif", "label.heic");
	gdTestAssert(fp != NULL);
	im = gdImageCreateFromHeif(fp);
	if (gdTestAssert(im != NULL))
		gdImageDestroy(im);
	fclose(fp);

	return gdNumFailures();
}
