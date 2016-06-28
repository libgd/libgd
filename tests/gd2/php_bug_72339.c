#include <stdio.h>
#include <stdlib.h>
#include "gd.h"

#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;

	fp = gdTestFileOpen("gdimagerotate/php_bug_64898.png");
	im = gdImageCreateFromGd2(fp);
	if (im == NULL) {
		return 0;
	} else {
		gdTestErrorMsg("Image should have failed to be loaded");
		return 0;
	}

}
