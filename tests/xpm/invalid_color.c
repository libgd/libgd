#include "gd.h"
#include "gdtest.h"
#include <stdlib.h>

int main(void) {
	char *path = gdTestFilePath("xpm/invalid_color.xpm");
	gdImagePtr im = gdImageCreateFromXpm(path);

	free(path);
	if (im != NULL) {
		gdImageDestroy(im);
		return 1;
	}
	return 0;
}
