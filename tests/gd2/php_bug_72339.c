#include <stdio.h>
#include <stdlib.h>
#include "gd.h"

#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	
	fp = gdTestFileOpen2("gd2", "php_bug_72339_exp.gd2");
	im = gdImageCreateFromGd2(fp);
	if (im == NULL) {
		return 0;
	} else {
		gdTestErrorMsg("Image should have failed to be loaded");
		return 1;
	}

}
