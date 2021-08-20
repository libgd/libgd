/* Exercise all scaling with all interpolation modes and ensure that
 * at least, something comes back. */

#include <stdio.h>

#include "gd.h"
#include "gdtest.h"

#define X 100
#define Y 100

#define NX 20
#define NY 20

int main() {
    unsigned int method, i;

    for(method = GD_DEFAULT; method < GD_METHOD_COUNT; method++) {   /* GD_WEIGHTED4 is unsupported. */
        gdImagePtr im[2];
        if (method == GD_WEIGHTED4) {
            continue;
        }

        im[0] = gdImageCreateTrueColor(X, Y);
        im[1] = gdImageCreatePalette(X, Y);

        for (i = 0; i < 2; i++) {
            gdImagePtr result;

            gdImageFilledRectangle(im[i], 0, 0, X-1, Y-1,
                                   gdImageColorExactAlpha(im[i], 255, 255, 255, 0));

            gdImageSetInterpolationMethod(im[i], method);

            gdTestAssert(im[i]->interpolation_id == gdImageGetInterpolationMethod(im[i])); /* No getter yet. */

            result = gdImageScale(im[i], NX, NY);
            gdTestAssert(result != NULL);
            gdTestAssert(result != im[i]);
            if (result == NULL) {

				gdTestErrorMsg("gdImageScale failed (method: %i, im:%i).\n", method, i);
				break;
			}
            if (!(result->sx == NX && result->sy == NY)) {
                gdTestErrorMsg("gdImageScale failed Dimensions (%i, %i) (%i, %i)(method: %i, im:%i).\n", result->sx, result->sy, NX, NY, method, i);
            }
            gdTestAssert(result->sx == NX && result->sy == NY);

            gdImageDestroy(result);
            gdImageDestroy(im[i]);
        }/* for */
    }/* for*/

    printf("%i\n", gdNumFailures());
    return gdNumFailures();
}/* main*/
