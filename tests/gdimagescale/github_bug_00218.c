#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im, dest;
	int status;

	im = gdImageCreate(5,5);
	dest = gdImageScale(im, 0, 1);
	if (dest != NULL) {
		gdTestErrorMsg("expecting NULL result\n");
		status = 1;
	} else {
		status = 0;
	}
	gdImageDestroy(im);

	return status;
}
