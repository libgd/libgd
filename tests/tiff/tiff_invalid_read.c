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
    char *buffer;
    size_t size;

    size = read_test_file(&buffer, basename);
    im = gdImageCreateFromTiffPtr(size, (void *) buffer);
    gdTestAssert(im == NULL);
    free(buffer);
}


static size_t read_test_file(char **buffer, char *basename)
{
    char *filename;
    FILE *fp;
    size_t exp_size, act_size;

    filename = gdTestFilePath2("tiff", basename);
    fp = fopen(filename, "rb");
    gdTestAssert(fp != NULL);

	fseek(fp, 0, SEEK_END);
	exp_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

    *buffer = malloc(exp_size);
    gdTestAssert(*buffer != NULL);
    act_size = fread(*buffer, sizeof(**buffer), exp_size, fp);
    gdTestAssert(act_size == exp_size);

    fclose(fp);
    free(filename);

    return act_size;
}
