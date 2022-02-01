/*
We're testing that reading corrupt TIFF files doesn't cause any memory issues,
and that the operation gracefully fails (i.e. gdImageCreateFromTiffPtr() returns
NULL).
*/

#include "gd.h"
#include "gdtest.h"


static void check_file(char *basename);
static size_t read_test_file(char **buffer, char *basename);


int main()
{
    check_file("tiff_invalid_read_1.tiff");
    check_file("tiff_invalid_read_2.tiff");
    check_file("tiff_invalid_read_3.tiff");

    return gdNumFailures();
}


static void check_file(char *basename)
{
    gdImagePtr im;
    char *buffer = NULL;
    size_t size;

    size = read_test_file(&buffer, basename);
    im = gdImageCreateFromTiffPtr(size, (void *) buffer);
    gdTestAssert(im == NULL);
    if (buffer != NULL) {
        free(buffer);
    }
}


static size_t read_test_file(char **buffer, char *basename)
{
    char *filename;
    FILE *fp;
    size_t exp_size = 0, act_size = -1;

    filename = gdTestFilePath2("tiff", basename);
    fp = fopen(filename, "rb");
    if (!gdTestAssert(fp != NULL)) goto fail3;

    if (fseek(fp, 0, SEEK_END) != 0) {
        gdTestAssert(1==0); // only increase num failures used as return values in main
        goto fail2;
    }

    exp_size = ftell(fp);
    if (exp_size <= 0) {
        gdTestAssert(1==0); // only increase num failures used as return values in main
        goto fail2;
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        gdTestAssert(1==0); // only increase num failures used as return values in main
        goto fail2;
    }

    *buffer = malloc(exp_size);
    if (*buffer != NULL) {
        act_size = fread(*buffer, sizeof(**buffer), exp_size, fp);
        gdTestAssert(act_size == exp_size);
    } else {
        gdTestAssert(0);
    }

fail2:
    fclose(fp);
fail3:
    free(filename);

    return act_size;
}
