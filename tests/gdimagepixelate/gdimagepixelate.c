#include "gd.h"
#include "gdtest.h"

#define WIDTH 12
#define BLOCK_SIZE 4

static const int expected_upperleft[][3] = {
	{0x000000, 0x040404, 0x080808},
	{0x303030, 0x343434, 0x383838},
	{0x606060, 0x646464, 0x686868}
};

static const int expected_average[][3] = {
	{0x131313, 0x171717, 0x1b1b1b},
	{0x434343, 0x474747, 0x4b4b4b},
	{0x737373, 0x777777, 0x7b7b7b},
};

#define SETUP_PIXELS(im) do {								\
		int x, y, i = 0;									\
		for (y = 0; y < (im)->sy; y++) {					\
			for (x = 0; x < (im)->sx; x++) {				\
				int p = gdImageColorResolve(im, i, i, i);	\
				gdImageSetPixel(im, x, y, p);				\
				i++;										\
			}												\
		}													\
	} while (0)

#define CHECK_PIXELS(im, expected) do {														  \
		int x, y;																			  \
		for (y = 0; y < (im)->sy; y++) {													  \
			for (x = 0; x < (im)->sx; x++) {												  \
				int p = gdImageGetPixel(im, x, y);											  \
				int r = ((expected)[y/BLOCK_SIZE][x/BLOCK_SIZE]>>16)&0xFF;					  \
				int g = ((expected)[y/BLOCK_SIZE][x/BLOCK_SIZE]>> 8)&0xFF;					  \
				int b = ((expected)[y/BLOCK_SIZE][x/BLOCK_SIZE]    )&0xFF; 					  \
				if (r != gdImageRed(im, p)) {												  \
					gdTestErrorMsg("Red %x is expected, but %x\n", r, gdImageRed(im, p));	  \
					return 0;																  \
				}																			  \
				if (g != gdImageGreen(im, p)) {												  \
					gdTestErrorMsg("Green %x is expected, but %x\n", g, gdImageGreen(im, p)); \
					return 0;																  \
				}																			  \
				if (b != gdImageBlue(im, p)) {												  \
					gdTestErrorMsg("Blue %x is expected, but %x\n", b, gdImageBlue(im, p));	  \
					return 0;																  \
				}																			  \
			}																				  \
		}																					  \
	} while (0)

static int testPixelate(gdImagePtr im)
{
	if (gdImagePixelate(im, -1, GD_PIXELATE_UPPERLEFT) != 0) return 0;
	if (gdImagePixelate(im, 1, GD_PIXELATE_UPPERLEFT) != 1) return 0;
	if (gdImagePixelate(im, 2, -1) != 0) return 0;

	SETUP_PIXELS(im);
	if (!gdImagePixelate(im, BLOCK_SIZE, GD_PIXELATE_UPPERLEFT)) return 0;
	CHECK_PIXELS(im, expected_upperleft);

	SETUP_PIXELS(im);
	if (!gdImagePixelate(im, BLOCK_SIZE, GD_PIXELATE_AVERAGE)) return 0;
	CHECK_PIXELS(im, expected_average);

	return 1;
}

int main()
{
	gdImagePtr im;

	im = gdImageCreate(WIDTH, WIDTH);
	if (!testPixelate(im)) {
		return 1;
	}
	gdImageDestroy(im);

	im = gdImageCreateTrueColor(WIDTH, WIDTH);
	if (!testPixelate(im)) {
		return 2;
	}
	gdImageDestroy(im);

	return 0;
}
