#include <stdio.h>
#include <stdlib.h>

#include "gd.h"
#include "gdtest.h"
#include "../test_config.h"

#define WIDTH 60
#define HEIGHT 50
#define LX (WIDTH/2)    // Line X
#define LY (HEIGHT/2)   // Line Y
#define HT 2            // Half of line-thickness


gdImagePtr mkwhite(int x, int y)
{
    gdImagePtr im;

	im = gdImageCreateTrueColor(x, y);
	gdImageFilledRectangle(im, 0, 0, x-1, y-1,
                           gdImageColorExactAlpha(im, 255, 255, 255, 0));

    gdTestAssert(im != NULL);

    gdImageSetInterpolationMethod(im, GD_BICUBIC);    // FP interp'n

    return im;
}/* mkwhite*/


gdImagePtr mkcross() {
    gdImagePtr im;
    int fg, n;

    im = mkwhite(WIDTH, HEIGHT);
    fg = gdImageColorAllocate(im, 0, 0, 0);

    for (n = -HT; n < HT; n++) {
        gdImageLine(im, LX-n, 0, LX-n, HEIGHT-1, fg);
        gdImageLine(im, 0, LY-n, WIDTH-1, LY-n, fg);
    }/* for */

    return im;
}/* mkcross*/




void
do_test() {

    gdTestAssertMsg(strchr("123",'2') != 0, "strchr() is not functional.\n",1);
    gdTestAssertMsg(strcasecmp("123abC","123Abc") == 0, "strcasecmp() is not functional.\n",1);
    
    int n;
    struct {
        const char *nm;     // Filename
        unsigned maxdiff;   // Maximum total pixel diff
        int required;       // 1 -> image type always supported, -1 -> skip it
        int readonly;       // 1 -> gd can only read this type
    } names[] = {
        {"img.png",     0,  0,  0},
        {"img.gif",     5,  1,  0},     // This seems to come from tc<->palette
        {"img.GIF",     5,  1,  0},     // Test for case insensitivity
        {"img.gd",      0,  1,  0}, 
        {"img.gd2",     0,  0,  0}, 
        {"img.jpg",    25,  0,  0},
        {"img.jpeg",   25,  0,  0},
        {"img.wbmp",    0,  1,  0},
        {"img.bmp",     0,  1,  0},
        {"img-ref.xpm", 0,  0,  1},
        
        // These break the test so I'm skipping them since the point
        // of this test is not those loaders.
        {"img-ref.xbm", 0, -1,  1},
        {"img-ref.tga", 0, -1,  1},
        {"img.webp",    0, -1,  0},
        {"img.tiff",    0, -1,  0},

        {NULL, 0}
    };

    for (n = 0; names[n].nm; n++) {
        gdImagePtr orig, copy;
        int status;
        char full_filename[255];

        /* Some image readers are buggy and crash the program so we
         * skip them.  Bug fixers should remove these from the list of
         * skipped items as bugs are fixed. */
        if (names[n].required < 0) {
            printf("Skipping test for '%s'.  FIX THIS!\n", names[n].nm);
            continue;
        }/* if */

        /* Skip this file if the current library build doesn't support
         * it.  (If it's one of the built-in types, *that* a different
         * problem; we assert that here.) */
        if (!gdSupportsFileType(names[n].nm, 0)) {
            gdTestAssertMsg(!names[n].required, "GD doesn't support required file type: %s\n", full_filename);
            continue;
        }/* if */

        orig = mkcross();

        
        #ifdef GDTEST_TOP_DIR 

        snprintf(full_filename, sizeof(full_filename), GDTEST_TOP_DIR"/gdimagefile/%s",
                 names[n].nm);

        #else
        /* Prepend the test directory; this is expected to be run in
         * the parent dir. */
        snprintf(full_filename, sizeof(full_filename), "gdimagefile/%s",
                 names[n].nm);
        #endif 


        /* Write the image unless writing is not supported. */
        if (!names[n].readonly) {
            status = gdImageFile(orig, full_filename);
            gdTestAssertMsg(status == GD_TRUE, "Failed to create %s\n", full_filename);
        }/* if */

        copy = gdImageCreateFromFile(full_filename);
        gdTestAssertMsg(!!copy, "Failed to load %s\n", full_filename);
        if (!copy) continue;

        /* Debug printf. */
        //printf("%s -> %d\n", full_filename, gdMaxPixelDiff(orig, copy));
        
        gdTestAssertMsg(gdMaxPixelDiff(orig, copy) <= names[n].maxdiff,"Pixels different on %s\n", full_filename, full_filename);

        if (!names[n].readonly) {
            status = remove(full_filename);
            gdTestAssertMsg(status == 0, "Failed to delete %s\n", full_filename);
        }/* if */

        gdImageDestroy(orig);
        gdImageDestroy(copy);
    }/* for */

}/* do_test*/


void
do_errortest() {
    gdImagePtr im;

    im = mkcross();

    gdTestAssert(!gdImageFile(im, "img.xpng"));
    gdTestAssert(!gdImageFile(im, "bobo"));
    gdTestAssert(!gdImageFile(im, "png"));
    gdTestAssert(!gdImageFile(im, ""));

    gdImageDestroy(im);
}/* do_errortest*/


int main(int argc, char **argv)
{

    do_test();
    do_errortest();

    return gdNumFailures();
}
