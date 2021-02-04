/**
 * Simple test case that confirms the failure of using `gdImageCreateFromHeif`
 * with a NULL pointer.
 */


#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;

	im = gdImageCreateFromHeif(NULL);
	if (!gdTestAssert(im == NULL))
		gdImageDestroy(im);
	gdImageHeif(im, NULL); /* noop safely */

	return gdNumFailures();
}
