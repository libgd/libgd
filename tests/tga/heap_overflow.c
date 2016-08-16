/**
 * Test that the crafted TGA file doesn't trigger OOB reads.
 */


#include "gd.h"
#include "gdtest.h"


static size_t read_test_file(char **buffer, char *basename);


int main()
{
    gdImagePtr im;
    char *buffer;
    size_t size;

    size = read_test_file(&buffer, "heap_overflow.tga");
    im = gdImageCreateFromTgaPtr(size, (void *) buffer);
    gdTestAssert(im == NULL);
    free(buffer);

    return gdNumFailures();
}


static size_t read_test_file(char **buffer, char *basename)
{
    char *filename;
    FILE *fp;
    size_t exp_size, act_size;

    filename = gdTestFilePath2("tga", basename);
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
