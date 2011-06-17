#include "gd.h"

#ifdef WIN32
# include <windows.h>
#else
# include <unistd.h>
#endif
#include <stdlib.h>
#include <time.h>

#define PIXEL_FUNCTION_DECLARE(f) int (*f)(gdImagePtr, int, int)

#define GET_PIXEL_FUNCTION(src)(src->trueColor ? gdImageGetTrueColorPixel : gdImageGetPixel)

#ifdef WIN32
# define GD_SCATTER_SEED() (unsigned int)(time(0) * GetCurrentProcessId())
#else
# define GD_SCATTER_SEED() (unsigned int)(time(0) * getpid())
#endif

BGD_DECLARE(int) gdImageScatter(gdImagePtr im, int sub, int plus)
{
	gdScatter s;

	s.sub  = sub;
	s.plus = plus;
	s.num_colors = 0;
	s.seed = GD_SCATTER_SEED();
	return gdImageScatterEx(im, &s);
}

BGD_DECLARE(int) gdImageScatterColor(gdImagePtr im, int sub, int plus, int colors[], unsigned int num_colors)
{
	gdScatter s;

	s.sub  = sub;
	s.plus = plus;
	s.colors = colors;
	s.num_colors = num_colors;
	s.seed = GD_SCATTER_SEED();
	return gdImageScatterEx(im, &s);
}

#define GD_SCATTER_MAIN() do {									\
		dest_x = (int) (x + ((rand() % (plus - sub)) + sub));	\
		dest_y = (int) (y + ((rand() % (plus - sub)) + sub));	\
																\
		if (!gdImageBoundsSafe(im, dest_x, dest_y)) {			\
			continue;											\
		}														\
																\
		pxl = f(im, x, y);										\
		new_pxl = f(im, dest_x, dest_y);						\
	} while (0)

BGD_DECLARE(int) gdImageScatterEx(gdImagePtr im, gdScatterPtr scatter)
{
	register int x, y;
	int dest_x, dest_y;
	int pxl, new_pxl;
	unsigned int n;
	int sub = scatter->sub, plus = scatter->plus;
	PIXEL_FUNCTION_DECLARE(f);

	if (plus == 0 && sub == 0) {
		return 1;
	} else if (sub >= plus) {
		return 0;
	}

	f = GET_PIXEL_FUNCTION(im);
	(void)srand(scatter->seed);

	if (scatter->num_colors) {
		for (y = 0; y < im->sy; y++) {
			for (x = 0; x < im->sx; x++) {
				GD_SCATTER_MAIN();

				for (n = 0; n < scatter->num_colors; n++) {
					if (pxl == scatter->colors[n]) {
						gdImageSetPixel(im, dest_x, dest_y, pxl);
						gdImageSetPixel(im, x, y, new_pxl);
					}
				}
			}
		}
	} else {
		for (y = 0; y < im->sy; y++) {
			for (x = 0; x < im->sx; x++) {
				GD_SCATTER_MAIN();

				gdImageSetPixel(im, dest_x, dest_y, pxl);
				gdImageSetPixel(im, x, y, new_pxl);
			}
		}
	}

	return 1;
}
