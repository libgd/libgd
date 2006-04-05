#include "gd.h"
#include <string.h>

int main(int argc, char *argv[])
{
	gdImagePtr im;
	int black;
	int white;
	int brect[8];
	int x, y;
	char *err;

	char *s = "Hello. Qyjpqg,"; /* String to draw. */
	double sz = 40.;
	char *f = "/usr/local/share/ttf/times.ttf"; /* TrueType font */
	
	/* obtain brect so that we can size the image */
	err = gdImageStringTTF((gdImagePtr)NULL,&brect[0],0,f,sz,0.,0,0,s);
	if (err) {fprintf(stderr,err); return 1;}

	/* create an image just big enough for the string */
	x = brect[2]-brect[6] + 6;
	y = brect[3]-brect[7] + 6;
	im = gdImageCreate(x,y);

	/* Background color (first allocated) */
	white = gdImageColorResolve(im, 255, 255, 255);
	black = gdImageColorResolve(im, 0, 0, 0);

	/* render the string, offset origin to center string*/
	x = 0-brect[6] + 3;
	y = 0-brect[7] + 3;
	err = gdImageStringTTF(im,&brect[0],black,f,sz,0.0,x,y,s);
	if (err) {fprintf(stderr,err); return 1;}

	/* Write img to stdout */
	gdImagePng(im, stdout);

	/* Destroy it */
	gdImageDestroy(im);

	return 0;
}	
