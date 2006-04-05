/* 
   Creation of wbmp image files with gd library
   gd_wbmp.c

   Copyright (C) Maurice Szmurlo --- T-SIT --- January 2000
   (Maurice.Szmurlo@info.unicaen.fr)

   Permission to use, copy, modify, and distribute this software and its
   documentation for any purpose and without fee is hereby granted, provided
   that the above copyright notice appear in all copies and that both that
   copyright notice and this permission notice appear in supporting
   documentation.  This software is provided "as is" without express or
   implied warranty.

   -----------------------------------------------------------------------------------------
   Parts od this code are inspired by  'pbmtowbmp.c' and 'wbmptopbm.c' by 
   Terje Sannum <terje@looplab.com>.
   **
   ** Permission to use, copy, modify, and distribute this software and its
   ** documentation for any purpose and without fee is hereby granted, provided
   ** that the above copyright notice appear in all copies and that both that
   ** copyright notice and this permission notice appear in supporting
   ** documentation.  This software is provided "as is" without express or
   ** implied warranty.
   **
   -----------------------------------------------------------------------------------------

   Todo:

   gdCreateFromWBMP function for reading WBMP files

   -----------------------------------------------------------------------------------------

   Compilation:
   
   * testing the mbi data structure:
   gcc -Wall -W -D__MBI_DEBUG__ gd_wbmp.c -o mbi -L/usr/local/lib -lgd
   
   * testing generation of wbmp images
   gcc -Wall -W -D__GD_WBMP_DEBUG__ gd_wbmp.c -o gd_wbmp -L/usr/local/lib -lgd -lpng
   
   * simply making the object file
   gcc -c -Wall -W gd_wbmp.c -o gd_wbmp.o
*/

#include <gd.h>
#include <gdfonts.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


/* *************************** Multi Byte Integer Funcionnalities  *************************** */

/* **************************************************
  An mbi is an array of bytes.
  For 32 bit integers, 5 bytes are sufficient.
*/
typedef unsigned char mbi_t[6];


/* **************************************************
  initialization of an mbi in order to be used by the int2mbi function
*/
static void mbiInit(mbi_t  mbi) {
  int i;
  mbi[0] = mbi[5] = (unsigned char)0;
  for(i=1; i<5; mbi[i++] = 0x80);
}


/*  **************************************************
   Displays the content of the mbi as a serie of hexa bytes.
   For debuging purpose only
*/
#ifdef __MBI_DEBUG__
static void mbiDisplay(mbi_t mbi, FILE *out) {
  unsigned int i;
  fprintf(out, "MBI:%d [", (unsigned int)mbi[0]);
  if(mbi[0] > 0)
	fprintf(out, "%X", (unsigned int)mbi[1]);
  for(i=2; i<=mbi[0]; i++)
	fprintf(out, ":%02X", (unsigned int)mbi[i]);
  fprintf(out, "]\n");
}
#endif /* __MBI_DEBUG__ */


/* **************************************************
  writes the bytes constituing the mbi on the stream 'out'
*/
static void mbiPrint(mbi_t mbi, gdIOCtx *out) {
  unsigned int i;
  for(i=1; i<=(unsigned int)mbi[0]; gdPutC(mbi[i++], out));
}


/* **************************************************
   Conversion routines:
   unsigned int        ->  multi byte integer
   multi byte integer  ->  unsigned int
*/
static void int2mbi(mbi_t mbi, unsigned int val) {
  unsigned int i, j;
  mbiInit(mbi);
  for(i=5; i>0; i--) {
	mbi[i] = mbi[i] | (val & 0x7F);
	val = val >> 7;
  }
  for(j=1; (mbi[j] & 0x7F) == 0 && j<=5; j++);
  mbi[0] = (unsigned char)(5-j+1);
  for(i=1; i<=mbi[0]; mbi[i++] = mbi[j++]);
}

static unsigned int mbi2int(mbi_t mbi) {
  unsigned int res=0,  i;
  for(i=0; i<(unsigned int)mbi[0]; i++)
	res = (res << 7) | (unsigned int) ( mbi[i+1] & 0x7F );
  return res;
}

#ifdef __MBI_DEBUG__
int main(void) {
  mbi_t mbi;
  fprintf(stdout, "testing Multi Byte Integers:\n");

  int2mbi(mbi, 0);
  mbiDisplay(mbi, stdout);
  fprintf(stdout, ">> = %u\n", mbi2int(mbi));

  /* 0xA0 and 0x60 are the two examples from the wbmp specs */
  int2mbi(mbi, 0xA0);
  mbiDisplay(mbi, stdout);
  fprintf(stdout, ">> = %u\n", mbi2int(mbi));
  
  int2mbi(mbi, 0x60);
  mbiDisplay(mbi, stdout);
  fprintf(stdout, ">> = %u\n", mbi2int(mbi));

  int2mbi(mbi, 1024+512);
  mbiDisplay(mbi, stdout);
  fprintf(stdout, ">> = %u\n", mbi2int(mbi));

  int2mbi(mbi, 1024+512+1);
  mbiDisplay(mbi, stdout);
  fprintf(stdout, ">> = %u\n", mbi2int(mbi));

  int2mbi(mbi, 775432);
  mbiDisplay(mbi, stdout);
  fprintf(stdout, ">> = %u\n", mbi2int(mbi));

  int2mbi(mbi, 405589432);
  mbiDisplay(mbi, stdout);
  fprintf(stdout, ">> = %u\n", mbi2int(mbi));

  int2mbi(mbi, UINT_MAX);
  mbiDisplay(mbi, stdout);
  fprintf(stdout, ">> = %u\n", mbi2int(mbi));

  fprintf(stdout, "\n");
  return 0;
}
#endif

/* *************************** GD interface *************************** */

/*
  Write the image as a wbmp file
  Parameters are:
  image:  gd image structure;
  fg:     the index of the foreground color. any other value will be considered 
          as background and will not be written
  out:    the stream where to write
*/
void gdImageWBMPCtx(gdImagePtr image, int fg, gdIOCtx *out) {
  
  int x, y;
  mbi_t mbi;

  /* wbmp images are type 0: B&W; no compression; empty FixHeaderFlield */
  gdPutC(0, out);					/* header */
  gdPutC(0, out);					/* FixHeaderFlield */

  /* width and height as mbi */
  int2mbi(mbi, gdImageSX(image));    /* width */
  mbiPrint(mbi, out);
  int2mbi(mbi, gdImageSY(image));    /* height */
  mbiPrint(mbi, out);

  /* 
	 now come the pixels as strings of bits padded with 0's at the end of the
	 lines, if necessary
  */
  for(y=0; y<gdImageSY(image); y++) {
	unsigned char nbPixs = 0;		/* how many pixels already written */
	unsigned char pixels = 0;	    /* the 8 pixels string */
	for(x=0; x<gdImageSX(image); x++) {
	  if(gdImageGetPixel(image, x, y) == fg) {
		pixels = pixels | (1 << (7-nbPixs)); 
	  }
	  nbPixs ++;
	  if(nbPixs == 8) { /* 8 pixels written -> write to file */
		gdPutC(pixels, out);
		pixels = 0;
		nbPixs = 0;
	  }
	}
	/* if there are pixels left to write, write them */
	if(nbPixs != 0)
	  gdPutC(pixels, out);
  }
}

/*
  Write the image as a wbmp file
  Parameters are:
  image:  gd image structure;
  fg:     the index of the foreground color. any other value will be considered 
          as background and will not be written
  out:    the stream where to write
*/
void gdImageWBMP(gdImagePtr image, int fg, FILE *out)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	gdImageWBMPCtx(im, out);
	out->free(out);
}

void* gdImageWBMPPtr(gdImagePtr im, int *size)
{
        void *rv;
        gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
        gdImageWBMPCtx(im, out);
        rv = gdDPExtractData(out, size);
        out->free(out);
        return rv;
}
 
#ifdef __GD_WBMP_DEBUG__
int  main(int argc, char **argv) {

  gdImagePtr image;
  int bg, fg;
  FILE *out;

  if(argc != 2) {
	fprintf(stderr, "usage %s <wbmp file>\n", argv[0]);
	exit(1);
  }
  
  fprintf(stdout, "testing gd generation of WBMP files\n");

  fprintf(stdout, "Opening WBMP file '%s'\n", argv[1]);
  if(!(out = fopen(argv[1], "w"))) {
	fprintf(stderr, "Cannot open out file '%s'\n", argv[1]);
	exit(1);
  }

  fprintf(stdout, "Creating GD image and drawing...y\n");
  if(!(image = gdImageCreate(60, 30))) {
	fprintf(stderr, "Cannot create image 30x60\n");
	exit(1);
  }

  /* don't really care about rgb values */
  bg = gdImageColorAllocate(image, 0, 0, 0);
  fg = gdImageColorAllocate(image, 255, 255, 255);

  /* draw something : */
  gdImageRectangle(image, 1, 1, gdImageSX(image)-2, gdImageSY(image)-2, fg);
  
  /* write something interesting */
  gdImageString(image, gdFontSmall, 3, 0, "Hello", fg);

  /* write the wbmp file */
  fprintf(stdout, "Writing WBMP file '%s':\n", argv[1]);
  gdImageWBMP(image, fg, out);
  fclose(out);
  fprintf(stdout, "Written.\nTry now to load the file '%s' with a WAP browser.\n", argv[1]);
  




  fprintf(stdout, "\n");
  return 0;
}
#endif /* __GD_WBMP_DEBUG__ */



  
