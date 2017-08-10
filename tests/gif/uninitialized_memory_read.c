/**
 * Test for uninitialized color map entries
 *
 * We're reading a crafted GIF which consists of 16x16 color blocks and which
 * has only two palette entries in the global color map, but uses other palette
 * indexes as well. We verify whether all undefined palette indexes produce the
 * color black.
 *
 * See also <CAKm_7a-AO++B6cXYWM_DtycPENG5WNWK7NSEvQ5OmZziMY_JyA@mail.gmail.com>
 * which had been sent to security@libgd.org.
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im;
    FILE *fp;
    int i, j, col;

    fp = gdTestFileOpen2("gif", "unitialized_memory_read.gif");
    gdTestAssert(fp != NULL);
    im = gdImageCreateFromGif(fp);
    gdTestAssert(im != NULL);
    fclose(fp);

    for (i = 0; i < gdImageSX(im); i += 16) {
        for (j = 0; j < gdImageSY(im); j += 16) {
            if (gdImageGetPixel(im, i, j) >= 2) {
                col = gdImageGetTrueColorPixel(im, i, j);
                gdTestAssertMsg(col == 0, "(%d,%d): expected color 0, but got %d\n", i, j, col);
            }
        }
    }

    gdImageDestroy(im);

    return gdNumFailures();
}
