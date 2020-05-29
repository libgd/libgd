#include <stdio.h>
#include <stdlib.h>
#include "gd.h"

#include "gdtest.h"


int main()
{
		gdRect bbox;
		gdRect area;
		double affine[6];

		affine[0] = -0.965926;
		affine[1] = -0.258819;
		affine[2] =  0.258819;
		affine[3] =  -0.965926;
		affine[4] =  0.000000;
		affine[5] =  0.000000;
		area.x = 0;
		area.y = 0;
		area.width = 262;
		area.height = 166;
		if (gdTransformAffineBoundingBox(&area, affine, &bbox) != GD_TRUE) {
			return 1;
		}
		if (!(bbox.x ==-253 &&  bbox.y ==-228 && bbox.width == 297 && bbox.height == 229)) {
			return 1;
		}

		return 0;
}
