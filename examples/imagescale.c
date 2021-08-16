/* Exercise all scaling with all interpolation modes and ensure that
 * at least, something comes back. */

#include <stdio.h>

#include "gd.h"

#define X 100
#define Y 100

#define NX 100
#define NY 100

gdImagePtr loadImage(const char *name)
{
	FILE *fp;
	gdImagePtr im;

	fp = fopen(name, "rb");
	if (!fp) {
		fprintf(stderr, "Can't open jpeg file\n");
		return NULL;
	}

	im = gdImageCreateFromPng(fp);
	fclose(fp);
	return im;
}

int savePngImage(gdImagePtr im, const char *name)
{
	FILE *fp;
	fp = fopen(name, "wb");
	if (!fp) {
		fprintf(stderr, "Can't save png image fromtiff.png\n");
		return 0;
	}
	gdImagePng(im, fp);
	fclose(fp);
	return 1;
}

int main() {
    unsigned int method;
    gdImagePtr src;
    src = loadImage("fillandstroke.png");
    if (!src) {
        printf("cannot open src image");
        exit(1);
    }
    int new_width = gdImageSX(src) *3;
    int new_height = gdImageSY(src) *3;
    for(method = GD_DEFAULT; method < GD_METHOD_COUNT; method++) {   /* GD_WEIGHTED4 is unsupported. */
        gdImagePtr result;
        if (method == GD_WEIGHTED4) {
            continue;
        }

        gdImageSetInterpolationMethod(src, method);
        printf("arg %i set: %i get %i\n", method, src->interpolation_id, gdImageGetInterpolationMethod(src));

        result = gdImageScale(src, new_width, new_height);
        if (result == NULL) {
            printf("gdImageScale failed (method: %i).\n", method);
            break;
        }
        if (result->sx != new_width || result->sy != new_height) {
            printf("missmatch width or height\n");
            gdImageDestroy(result);
            gdImageDestroy(src);
            exit(1);
        }

        char filename[255];
        sprintf(filename, "scale_%i.png", method);
        savePngImage(result, filename);
        gdImageDestroy(result);
    }/* for*/
    gdImageDestroy(src);

    return 0;
}
