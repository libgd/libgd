#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

/* To check memory leaks, run such as 'valgrind --leak-check=full ./bug00185' */
int
main()
{
	gdImagePtr im;
	char *path;

	path = gdTestFilePath("xpm/bug00185.xpm");
	im = gdImageCreateFromXpm(path);
	free(path);
	if (!im) return 1;
	gdImageDestroy(im);

	path = gdTestFilePath("xpm/bug00185_damaged.xpm");
	im = gdImageCreateFromXpm(path);
	free(path);
	if (!im) return 0;
	gdImageDestroy(im);

	return 2;
}
