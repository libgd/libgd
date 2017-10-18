/**
 * Regression test for <https://github.com/libgd/libgd/issues/295>.
 *
 * We're testing that reading a PNG which triggers a libpng warning actually
 * calls gdPngWarningHandler() once, and ignores other calls to the supplied
 * error handler (which are supposed to be follow up errors).
 */


#include "gd.h"
#include "gd_errors.h"
#include "gdtest.h"
#include <string.h>


#define WARNING_MSG "gd-png: libpng warning: %s\n"


static int num_png_warnings = 0;


static void error_handler(int priority, const char *format, va_list args)
{
    if (priority == GD_WARNING && !strcmp(format, WARNING_MSG)) {
        num_png_warnings++;
    }
}


int main()
{
    gdImagePtr im;
    FILE *fp;

    gdSetErrorMethod(error_handler);
    fp = gdTestFileOpen2("png", "bug00295.png");

    im = gdImageCreateFromPng(fp);
    gdTestAssert(im == NULL);

    fclose(fp);

    gdTestAssert(num_png_warnings == 1);

    return gdNumFailures();
}
