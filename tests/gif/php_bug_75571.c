/**
 * Test that GIF reading does not loop infinitely
 *
 * We are reading a crafted GIF image which has been truncated.  This would
 * trigger an infinite loop formerly, but know bails out early, returning
 * NULL from gdImageCreateFromGif().
 *
 * See also https://bugs.php.net/bug.php?id=75571.
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr im;
    FILE *fp;

    fp = gdTestFileOpen2("gif", "php_bug_75571.gif");
    gdTestAssert(fp != NULL);
    im = gdImageCreateFromGif(fp);
    gdTestAssert(im == NULL);
    fclose(fp);

    return gdNumFailures();
}
