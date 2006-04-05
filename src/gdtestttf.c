#include "gd.h"
#include <string.h>

#define PI 3.141592
#define DEG2RAD(x) ((x)*PI/180.)

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

#define MAX4(x,y,z,w) \
	((MAX((x),(y))) > (MAX((z),(w))) ? (MAX((x),(y))) : (MAX((z),(w))))
#define MIN4(x,y,z,w) \
	((MIN((x),(y))) < (MIN((z),(w))) ? (MIN((x),(y))) : (MIN((z),(w))))

#define MAXX(x) MAX4(x[0],x[2],x[4],x[6])
#define MINX(x) MIN4(x[0],x[2],x[4],x[6])
#define MAXY(x) MAX4(x[1],x[3],x[5],x[7])
#define MINY(x) MIN4(x[1],x[3],x[5],x[7])

int main(int argc, char *argv[])
{
#ifndef HAVE_LIBTTF
	fprintf(stderr, "gd was not compiled with HAVE_LIBTTF defined.\n");
	fprintf(stderr, "Install the FreeType library, including the\n");
	fprintf(stderr, "header files. Then edit the gd Makefile, type\n");
	fprintf(stderr, "make clean, and type make again.\n");
	return 1;
#else
	gdImagePtr im;
	int black;
	int white;
	int brect[8];
	int x, y;
	char *err;

#ifdef JISX0208
	char *s = "Hello. ‚±‚ñ‚É‚¿‚Í Qyjpqg,"; /* String to draw. */
#else
	char *s = "Hello. Qyjpqg,"; /* String to draw. */
#endif
	double sz = 40.;
	double angle = DEG2RAD(-90);
#ifdef JISX0208
	char *f = "/usr/openwin/lib/locale/ja/X11/fonts/TT/HG-MinchoL.ttf"; /* UNICODE */
	/* char *f = "/usr/local/lib/fonts/truetype/DynaFont/dfpop1.ttf"; */ /* SJIS */
#else
	char *f = "/usr/local/lib/fonts/truetype/misc/times.ttf"; /* TrueType font */
#endif
	
	/* obtain brect so that we can size the image */
	err = gdImageStringTTF((gdImagePtr)NULL,&brect[0],0,f,sz,angle,0,0,s);
	if (err) {fprintf(stderr,err); return 1;}

	/* create an image just big enough for the string */
	x = MAXX(brect) - MINX(brect) + 6;
	y = MAXY(brect) - MINY(brect) + 6;
	im = gdImageCreate(x,y);

	/* Background color (first allocated) */
	white = gdImageColorResolve(im, 255, 255, 255);
	black = gdImageColorResolve(im, 0, 0, 0);

	/* render the string, offset origin to center string*/
	x = 0 - MINX(brect) + 3;
	y = 0 - MINY(brect) + 3;
	err = gdImageStringTTF(im,&brect[0],black,f,sz,angle,x,y,s);
	if (err) {fprintf(stderr,err); return 1;}

	/* Write img to stdout */
	gdImagePng(im, stdout);

	/* Destroy it */
	gdImageDestroy(im);

	return 0;
#endif /* HAVE_TTF */
}	
