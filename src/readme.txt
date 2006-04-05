
   SEE index.html FOR A MUCH MORE USEFUL HYPERTEXT VERSION OF THIS DOCUMENT.
   SEE index.html FOR A MUCH MORE USEFUL HYPERTEXT VERSION OF THIS DOCUMENT.
   SEE index.html FOR A MUCH MORE USEFUL HYPERTEXT VERSION OF THIS DOCUMENT.


                                    gd 1.6
                                       
A graphics library for fast image creation

Follow this link to the [1]latest version of this document.

  Table of Contents
  
     * [2]Credits and license terms
     * [3]What's new in version 1.6?
     * [4]What is gd?
     * [5]What if I want to use another programming language?
     * [6]What else do I need to use gd?
     * [7]How do I get gd?
     * [8]How do I build gd?
     * [9]gd basics: using gd in your program
     * [10]webgif: a useful example
     * [11]Function and type reference by category
     * [12]About the additional .gd image file format
     * [13]Please tell us you're using gd!
     * [14]If you have problems
     * [15]Alphabetical quick index
       
   [16]Up to the Boutell.Com, Inc. Home Page
   
  Credits and license terms
  
   In order to resolve any possible confusion regarding the authorship of
   gd, the following copyright statement covers all of the authors who
   have required such a statement. _If you are aware of any oversights in
   this copyright notice, please contact [17]Thomas Boutell who will be
   pleased to correct them._

COPYRIGHT STATEMENT FOLLOWS THIS LINE

     Portions copyright 1994, 1995, 1996, 1997, 1998, 1999, by Cold
     Spring Harbor Laboratory. Funded under Grant P41-RR02188 by the
     National Institutes of Health.
     
     Portions copyright 1996, 1997, 1998, 1999, by Boutell.Com, Inc.
     
     Portions relating to GD2 format copyright 1999 Philip Warner.
     
     _Permission has been granted to copy and distribute gd in any
     context, including a commercial application, provided that this
     notice is present in user-accessible supporting documentation._
     
     This does not affect your ownership of the derived work itself, and
     the intent is to assure proper credit for the authors of gd, not to
     interfere with your productive use of gd. If you have questions,
     ask. "Derived works" includes all programs that utilize the
     library. Credit must be given in user-accessible documentation.
     
     Permission to use, copy, modify, and distribute this software and
     its documentation for any purpose and without fee is hereby
     granted, provided that the above copyright notice appear in all
     copies and that both that copyright notice and this permission
     notice appear in supporting documentation. This software is
     provided "as is" without express or implied warranty.
     
     Although their code does not appear in gd 1.6, the authors wish to
     thank David Koblas, David Rowley, and Hutchison Avenue Software
     Corporation for their prior contributions.
     
     Permission to use, copy, modify, and distribute this software and
     its documentation for any purpose and without fee is hereby
     granted, provided that the above copyright notice appear in all
     copies and that both that copyright notice and this permission
     notice appear in supporting documentation. _This software is
     provided "AS IS."_ The copyright holders disclaim all warranties,
     either express or implied, including but not limited to implied
     warranties of merchantability and fitness for a particular purpose,
     with respect to this code and accompanying documentation.
     
END OF COPYRIGHT STATEMENT

  What is gd?
  
   gd is a graphics library. It allows your code to quickly draw images
   complete with lines, arcs, text, multiple colors, cut and paste from
   other images, and flood fills, and write out the result as a .PNG
   file. This is particularly useful in World Wide Web applications,
   where .PNG is one of the formats accepted for inline images by most
   browsers.
   
   gd is not a paint program. If you are looking for a paint program, you
   are looking in the wrong place. If you are not a programmer, you are
   looking in the wrong place.
   
   gd does not provide for every possible desirable graphics operation.
   It is not necessary or desirable for gd to become a kitchen-sink
   graphics package, but version 1.6 incorporates most of the commonly
   requested features for an 8-bit 2D package. Support for scalable
   fonts, and truecolor images, JPEG and truecolor PNG is planned for
   version 2.0.
   
  What if I want to use another programming language?
  
    Perl
    
   gd can also be used from Perl, courtesy of Lincoln Stein's [18]GD.pm
   library, which uses gd as the basis for a set of Perl 5.x classes. Not
   yet updated for gd 1.6. be compatible.
   
    Any Language
    
   There are, at the moment, at least three simple interpreters that
   perform gd operations. You can output the desired commands to a simple
   text file from whatever scripting language you prefer to use, then
   invoke the interpreter.
   
   These packages have not been updated to gd 1.6 as of this writing.
     * [19]tgd, by Bradley K. Sherman
     * [20]fly, by Martin Gleeson
       
  What's new in version 1.6?
  
   Version 1.6 features the following changes:
   
   _Support for 8-bit palette PNG images has been added. Support for GIF
   has been removed._ This step was taken to completely avoid the legal
   controversy regarding the LZW compression algorithm used in GIF.
   Unisys holds a patent which is relevant to LZW compression. PNG is a
   superior image format in any case. Now that PNG is supported by both
   Microsoft Internet Explorer and Netscape (in their recent releases),
   we highly recommend that GD users upgrade in order to get
   well-compressed images in a format which is legally unemcumbered.
   
  What's new in version 1.5?
  
   Version 1.5 featured the following changes:
   
   _New GD2 format_
          An improvement over the GD format, the GD2 format uses the zlib
          compression library to compress the image in chunks. This
          results in file sizes comparable to GIFs, with the ability to
          access parts of large images without having to read the entire
          image into memory.
          
          This format also supports version numbers and rudimentary
          validity checks, so it should be more 'supportable' than the
          previous GD format.
          
   _Re-arranged source files_
          gd.c has been broken into constituant parts: io, gif, gd, gd2
          and graphics functions are now in separate files.
          
   _Extended I/O capabilities._
          The source/sink feature has been extended to support GD2 file
          formats (which require seek/tell functions), and to allow more
          general non-file I/O.
          
   _Better support for Lincoln Stein's Perl Module_
          The new gdImage*Ptr function returns the chosen format stored
          in a block of memory. This can be directly used by the GD perl
          module.
          
   _Added functions_
          gdImageCreateFromGd2Part - allows retrieval of part of an image
          (good for huge images, like maps),
          gdImagePaletteCopy - Copies a palette from one image to
          another, doing it's best to match the colors in the target
          image to the colors in the source palette.
          gdImageGd2, gdImageCreateFromGd2 - Support for new format
          gdImageCopyMerge - Merges two images (useful to highlight part
          of an image)
          gdImageCopyMergeGray - Similar to gdImageCopyMerge, but tries
          to preserve source image hue.
          gdImagePngPtr, gdImageGdPtr, gdImageGd2Ptr - return memort
          blocks for each type of image.
          gdImageCreateFromPngCtx, gdImageCreateFromGdCtx,
          gdImageCreateFromGd2Ctx, gdImageCreateFromGd2PartCtx - Support
          for new I/O context.
          
   _NOTE:_ In fairness to Thomas Boutell, any bug/problems with any of
   the above features should probably be reported to [21]Philip Warner.
   
  What's new in version 1.4?
  
   Version 1.4 features the following changes:
   
   Fixed polygon fill routine (again)
          Thanks to Kirsten Schulz, version 1.4 is able to fill numerous
          types of polygons that caused problems with previous releases,
          including version 1.3.
          
   Support for alternate data sources
          Programmers who wish to load a GIF from something other than a
          stdio FILE * stream can use the new
          [22]gdImageCreateFromPngSource function.
          
   Support for alternate data destinations
          Programmers who wish to write a GIF to something other than a
          stdio FILE * stream can use the new [23]gdImagePngToSink
          function.
          
   More tolerant when reading GIFs
          Version 1.4 does not crash when reading certain animated GIFs,
          although it still only reads the first frame. Version 1.4 also
          has overflow testing code to prevent crashes when reading
          damaged GIFs.
          
  What's new in version 1.3?
  
   Version 1.3 features the following changes:
   
   Non-LZW-based GIF compression code
          Version 1.3 contained GIF compression code that uses simple Run
          Length Encoding instead of LZW compression, while still
          retaining compatibility with normal LZW-based GIF decoders
          (your browser will still like your GIFs). _LZW compression is
          patented by Unisys. We are currently reevaluating the approach
          taken by gd 1.3. The current release of gd does not support
          this approach. We recommend that you use the current release,
          and generate PNG images._ Thanks to Hutchison Avenue Software
          Corporation for contributing the RLE GIF code.
          
   8-bit fonts, and 8-bit font support
          This improves support for European languages. Thanks are due to
          Honza Pazdziora and also to Jan Pazdziora . Also see the
          provided bdftogd Perl script if you wish to convert fixed-width
          X11 fonts to gd fonts.
          
   16-bit font support (no fonts provided)
          Although no such fonts are provided in the distribution, fonts
          containing more than 256 characters should work if the
          gdImageString16 and gdImageStringUp16 routines are used.
          
   Improvements to the "webgif" example/utility
          The "webgif" utility is now a slightly more useful application.
          Thanks to Brian Dowling for this code.
          
   Corrections to the color resolution field of GIF output
          Thanks to Bruno Aureli.
          
   Fixed polygon fills
          A one-line patch for the infamous polygon fill bug, courtesy of
          Jim Mason. I believe this fix is sufficient. However, if you
          find a situation where polygon fills still fail to behave
          properly, please send code that demonstrates the problem, _and_
          a fix if you have one. Verifying the fix is important.
          
   Row-major, not column-major
          Internally, gd now represents the array of pixels as an array
          of rows of pixels, rather than an array of columns of pixels.
          This improves the performance of compression and decompression
          routines slightly, because horizontally adjacent pixels are now
          next to each other in memory. _This should not affect properly
          written gd applications, but applications that directly
          manipulate the pixels array will require changes._
          
  What else do I need to use gd?
  
   To use gd, you will need an ANSI C compiler. _All popular Windows 95
   and NT C compilers are ANSI C compliant._ Any full-ANSI-standard C
   compiler should be adequate. _The cc compiler released with SunOS
   4.1.3 is not an ANSI C compiler. Most Unix users who do not already
   have gcc should get it. gcc is free, ANSI compliant and a de facto
   industry standard. Ask your ISP why it is missing._
   
   As of version 1.6, you also need the zlib compression library, and the
   libpng library. zlib is available for a variety of platforms from
   [24]the zlib web site. libpng is available for a variety of platforms
   from [25]the PNG web site.
   
   You will also want a PNG viewer, if you do not already have one for
   your system, since you will need a good way to check the results of
   your work. Netscape 4.04 and higher, and Microsoft Internet Explorer
   4.0 or higher, both support PNG. For some purposes you might be
   happier with a package like Lview Pro for Windows or xv for X. There
   are PNG viewers available for every graphics-capable modern operating
   system, so consult newsgroups relevant to your particular system.
   
  How do I get gd?
  
    By HTTP
    
     * [26]Gzipped Tar File (Unix)
     * [27].ZIP File (Windows)
       
    By FTP
    
     * [28]Gzipped Tar File (Unix)
     * [29].ZIP File (Windows)
       
  How do I build gd?
  
   In order to build gd, you must first unpack the archive you have
   downloaded. If you are not familiar with tar and gunzip (Unix) or ZIP
   (Windows), please consult with an experienced user of your system.
   Sorry, we cannot answer questions about basic Internet skills.
   
   Unpacking the archive will produce a directory called "gd1.6".
   
    For Unix
    
   cd to the gd1.6 directory and examine the Makefile, which you will
   probably need to change slightly depending on your operating system
   and your needs.
   
    For Windows, Mac, Et Cetera
    
   Create a project using your favorite programming environment. Copy all
   of the gd files to the project directory. Add gd.c to your project.
   Add other source files as appropriate. Learning the basic skills of
   creating projects with your chosen C environment is up to you.
   
   Now, to build the demonstration program, just type "make gddemo" if
   you are working in a command-line environment, or build a project that
   includes gddemo.c if you are using a graphical environment. If all
   goes well, the program "gddemo" will be compiled and linked without
   incident. Depending on your system you may need to edit the Makefile.
   Understanding the basic techniques of compiling and linking programs
   on your system is up to you.
   
   You have now built a demonstration program which shows off the
   capabilities of gd. To see it in action, type "gddemo".
   
   gddemo should execute without incident, creating the file demoout.png.
   (Note there is also a file named demoin.png, which is provided in the
   package as part of the demonstration.)
   
   Display demoout.png in your PNG viewer. The image should be 128x128
   pixels and should contain an image of the space shuttle with quite a
   lot of graphical elements drawn on top of it.
   
   (If you are missing the demoin.png file, the other items should appear
   anyway.)
   
   Look at demoin.png to see the original space shuttle image which was
   scaled and copied into the output image.
   
  gd basics: using gd in your program
  
   gd lets you create PNG images on the fly. To use gd in your program,
   include the file gd.h, and link with the libgd.a library produced by
   "make libgd.a", under Unix. Under other operating systems you will add
   gd.c to your own project.
   
   If you want to use the provided fonts, include gdfontt.h, gdfonts.h,
   gdfontmb.h, gdfontl.h and/or gdfontg.h. If you are not using the
   provided Makefile and/or a library-based approach, be sure to include
   the source modules as well in your project. (They may be too large for
   16-bit memory models, that is, 16-bit DOS and Windows.)
   
   Here is a short example program. _(For a more advanced example, see
   gddemo.c, included in the distribution. gddemo.c is NOT the same
   program; it demonstrates additional features!)_
   
/* Bring in gd library functions */
#include "gd.h"

/* Bring in standard I/O so we can output the PNG to a file */
#include <stdio.h>

int main() {
        /* Declare the image */
        [30]gdImagePtr im;
        /* Declare an output file */
        FILE *out;
        /* Declare color indexes */
        int black;
        int white;

        /* Allocate the image: 64 pixels across by 64 pixels tall */
        im = [31]gdImageCreate(64, 64);

        /* Allocate the color black (red, green and blue all minimum).
                Since this is the first color in a new image, it will
                be the background color. */
        black = [32]gdImageColorAllocate(im, 0, 0, 0);

        /* Allocate the color white (red, green and blue all maximum). */
        white = [33]gdImageColorAllocate(im, 255, 255, 255);
        
        /* Draw a line from the upper left to the lower right,
                using white color index. */
        [34]gdImageLine(im, 0, 0, 63, 63, white);

        /* Open a file for writing. "wb" means "write binary", important
                under MSDOS, harmless under Unix. */
        out = fopen("test.png", "wb");

        /* Output the image to the disk file. */
        [35]gdImagePng(im, out);

        /* Close the file. */
        fclose(out);

        /* Destroy the image in memory. */
        [36]gdImageDestroy(im);
}

   When executed, this program creates an image, allocates two colors
   (the first color allocated becomes the background color), draws a
   diagonal line (note that 0, 0 is the upper left corner), writes the
   image to a PNG file, and destroys the image.
   
   The above example program should give you an idea of how the package
   works. gd provides many additional functions, which are listed in the
   following reference chapters, complete with code snippets
   demonstrating each. There is also an [37]alphabetical index.
   
  Webpng: a more powerful gd example
  
   Webpng is a simple utility program to manipulate PNGs from the command
   line. It is written for Unix and similar command-line systems, but
   should be easily adapted for other environments. Webpng allows you to
   set transparency and interlacing and output interesting information
   about the PNG in question.
   
   webpng.c is provided in the distribution. Unix users can simply type
   "make webpng" to compile the program. Type "webpng" with no arguments
   to see the available options.
   
Function and type reference

     * [38]Types
     * [39]Image creation, destruction, loading and saving
     * [40]Drawing, styling, brushing, tiling and filling functions
     * [41]Query functions (not color-related)
     * [42]Font and text-handling functions
     * [43]Color handling functions
     * [44]Copying and resizing functions
     * [45]Miscellaneous Functions
     * [46]Constants
       
  Types
  
   gdImage_(TYPE)_
          The data structure in which gd stores images. [47]gdImageCreate
          returns a pointer to this type, and the other functions expect
          to receive a pointer to this type as their first argument. You
          may read the members sx (size on X axis), sy (size on Y axis),
          colorsTotal (total colors), red (red component of colors; an
          array of 256 integers between 0 and 255), green (green
          component of colors, as above), blue (blue component of colors,
          as above), and transparent (index of transparent color, -1 if
          none); please do so using the macros provided. Do NOT set the
          members directly from your code; use the functions provided.
          

typedef struct {
        unsigned char ** pixels;
        int sx;
        int sy;
        int colorsTotal;
        int red[gdMaxColors];
        int green[gdMaxColors];
        int blue[gdMaxColors];
        int open[gdMaxColors];
        int transparent;
} gdImage;

   gdImagePtr _(TYPE)_
          A pointer to an image structure. [48]gdImageCreate returns this
          type, and the other functions expect it as the first argument.
          
   gdFont _(TYPE)_
          A font structure. Used to declare the characteristics of a
          font. Plese see the files gdfontl.c and gdfontl.h for an
          example of the proper declaration of this structure. You can
          provide your own font data by providing such a structure and
          the associated pixel array. You can determine the width and
          height of a single character in a font by examining the w and h
          members of the structure. If you will not be creating your own
          fonts, you will not need to concern yourself with the rest of
          the components of this structure.
          

typedef struct {
        /* # of characters in font */
        int nchars;
        /* First character is numbered... (usually 32 = space) */
        int offset;
        /* Character width and height */
        int w;
        int h;
        /* Font data; array of characters, one row after another.
                Easily included in code, also easily loaded from
                data files. */
        char *data;
} gdFont;

   gdFontPtr _(TYPE)_
          A pointer to a font structure. Text-output functions expect
          these as their second argument, following the [49]gdImagePtr
          argument. Two such pointers are declared in the provided
          include files gdfonts.h and gdfontl.h.
          
   gdPoint _(TYPE)_
          Represents a point in the coordinate space of the image; used
          by [50]gdImagePolygon and [51]gdImageFilledPolygon.
          

typedef struct {
        int x, y;
} gdPoint, *gdPointPtr;

   gdPointPtr _(TYPE)_
          A pointer to a [52]gdPoint structure; passed as an argument to
          [53]gdImagePolygon and [54]gdImageFilledPolygon.
          
   gdSource _(TYPE)_

typedef struct {
        int (*source) (void *context, char *buffer, int len);
        void *context;
} gdSource, *gdSourcePtr;

   Represents a source from which a PNG can be read. Programmers who do
   not wish to read PNGs from a file can provide their own alternate
   input mechanism, using the [55]gdImageCreateFromPngSource function.
   See the documentation of that function for an example of the proper
   use of this type.
   
   gdSink _(TYPE)_

typedef struct {
        int (*sink) (void *context, char *buffer, int len);
        void *context;
} gdSink, *gdSinkPtr;

   Represents a "sink" (destination) to which a PNG can be written.
   Programmers who do not wish to write PNGs to a file can provide their
   own alternate output mechanism, using the [56]gdImagePngToSink
   function. See the documentation of that function for an example of the
   proper use of this type.
   
  Image creation, destruction, loading and saving
  
   gdImageCreate(sx, sy) _(FUNCTION)_
          gdImageCreate is called to create images. Invoke gdImageCreate
          with the x and y dimensions of the desired image. gdImageCreate
          returns a [57]gdImagePtr to the new image, or NULL if unable to
          allocate the image. The image must eventually be destroyed
          using [58]gdImageDestroy().
          

... inside a function ...
[59]gdImagePtr im;
im = gdImageCreate(64, 64);
/* ... Use the image ... */
[60]gdImageDestroy(im);

   gdImageCreateFromPng(FILE *in) _(FUNCTION)_
          gdImageCreateFromPngCtx([61]gdIOCtx *in) _(FUNCTION)_
          
          
          gdImageCreateFromPng is called to load images from PNG format
          files. Invoke gdImageCreateFromPng with an already opened
          pointer to a file containing the desired image.
          gdImageCreateFromPng returns a [62]gdImagePtr to the new image,
          or NULL if unable to load the image (most often because the
          file is corrupt or does not contain a PNG image).
          gdImageCreateFromPng does _not_ close the file. You can inspect
          the sx and sy members of the image to determine its size. The
          image must eventually be destroyed using [63]gdImageDestroy().
          

[64]gdImagePtr im;
... inside a function ...
FILE *in;
in = fopen("mypng.png", "rb");
im = gdImageCreateFromPng(in);
fclose(in);
/* ... Use the image ... */
[65]gdImageDestroy(im);

   gdImageCreateFromPngSource(gdSourcePtr in) _(FUNCTION)_
          gdImageCreateFromPngSource is called to load a PNG from a data
          source other than a file. Usage is very similar to the
          [66]gdImageCreateFromPng function, except that the programmer
          provides a custom data source.
          
          The programmer must write an input function which accepts a
          context pointer, a buffer, and a number of bytes to be read as
          arguments. This function must read the number of bytes
          requested, unless the end of the file has been reached, in
          which case the function should return zero, or an error has
          occurred, in which case the function should return -1. The
          programmer then creates a [67]gdSource structure and sets the
          source pointer to the input function and the context pointer to
          any value which is useful to the programmer.
          
          The example below implements [68]gdImageCreateFromPng by
          creating a custom data source and invoking
          gdImageCreateFromPngSource.
          

static int freadWrapper(void *context, char *buf, int len);

gdImagePtr gdImageCreateFromPng(FILE *in)
{
        gdSource s;
        s.source = freadWrapper;
        s.context = in;
        return gdImageCreateFromPngSource(&s);
}

static int freadWrapper(void *context, char *buf, int len)
{
        int got = fread(buf, 1, len, (FILE *) context);
        return got;
}

   gdImageCreateFromGd(FILE *in) _(FUNCTION)_
          gdImageCreateFromGdCtx([69]gdIOCtx *in) _(FUNCTION)_
          
          
          gdImageCreateFromGd is called to load images from gd format
          files. Invoke gdImageCreateFromGd with an already opened
          pointer to a file containing the desired image in the [70]gd
          file format, which is specific to gd and intended for very fast
          loading. (It is _not_ intended for compression; for
          compression, use PNG.) gdImageCreateFromGd returns a
          [71]gdImagePtr to the new image, or NULL if unable to load the
          image (most often because the file is corrupt or does not
          contain a gd format image). gdImageCreateFromGd does _not_
          close the file. You can inspect the sx and sy members of the
          image to determine its size. The image must eventually be
          destroyed using [72]gdImageDestroy().
          

... inside a function ...
[73]gdImagePtr im;
FILE *in;
in = fopen("mygd.gd", "rb");
im = gdImageCreateFromGd(in);
fclose(in);
/* ... Use the image ... */
[74]gdImageDestroy(im);

   gdImageCreateFromGd2(FILE *in) _(FUNCTION)_
          gdImageCreateFromGd2Ctx([75]gdIOCtx *in) _(FUNCTION)_
          
          
          gdImageCreateFromGd2 is called to load images from gd2 format
          files. Invoke gdImageCreateFromGd2 with an already opened
          pointer to a file containing the desired image in the [76]gd2
          file format, which is specific to gd2 and intended for fast
          loading of parts of large images. (It is a compressed format,
          but generally not as good a LZW compression).
          gdImageCreateFromGd returns a [77]gdImagePtr to the new image,
          or NULL if unable to load the image (most often because the
          file is corrupt or does not contain a gd format image).
          gdImageCreateFromGd2 does _not_ close the file. You can inspect
          the sx and sy members of the image to determine its size. The
          image must eventually be destroyed using [78]gdImageDestroy().
          

... inside a function ...
[79]gdImagePtr im;
FILE *in;
in = fopen("mygd.gd2", "rb");
im = gdImageCreateFromGd2(in);
fclose(in);
/* ... Use the image ... */
[80]gdImageDestroy(im);

   gdImageCreateFromGd2Part(FILE *in, int srcX, int srcY, int w, int h)
          _(FUNCTION)_
          gdImageCreateFromGd2PartCtx([81]gdIOCtx *in) _(FUNCTION)_
          
          
          gdImageCreateFromGd2Part is called to load parts of images from
          [82]gd2 format files. Invoked in the same way as
          [83]gdImageCreateFromGd2, but with extra parameters indicating
          the source (x, y) and width/height of the desired image.
          gdImageCreateFromGd2Part returns a [84]gdImagePtr to the new
          image, or NULL if unable to load the image. The image must
          eventually be destroyed using [85]gdImageDestroy().
          
   gdImageCreateFromXbm(FILE *in) _(FUNCTION)_
          gdImageCreateFromXbm is called to load images from X bitmap
          format files. Invoke gdImageCreateFromXbm with an already
          opened pointer to a file containing the desired image.
          gdImageCreateFromXbm returns a [86]gdImagePtr to the new image,
          or NULL if unable to load the image (most often because the
          file is corrupt or does not contain an X bitmap format image).
          gdImageCreateFromXbm does _not_ close the file. You can inspect
          the sx and sy members of the image to determine its size. The
          image must eventually be destroyed using [87]gdImageDestroy().
          

... inside a function ...
[88]gdImagePtr im;
FILE *in;
in = fopen("myxbm.xbm", "rb");
im = gdImageCreateFromXbm(in);
fclose(in);
/* ... Use the image ... */
[89]gdImageDestroy(im);

   gdImageDestroy(gdImagePtr im) _(FUNCTION)_
          gdImageDestroy is used to free the memory associated with an
          image. It is important to invoke gdImageDestroy before exiting
          your program or assigning a new image to a [90]gdImagePtr
          variable.
          

... inside a function ...
[91]gdImagePtr im;
im = [92]gdImageCreate(10, 10);
/* ... Use the image ... */
/* Now destroy it */
[93]gdImageDestroy(im);

   void gdImagePng(gdImagePtr im, FILE *out) _(FUNCTION)_
          gdImagePng outputs the specified image to the specified file in
          PNG format. The file must be open for writing. Under MSDOS and
          all versions of Windows, it is important to use "wb" as opposed
          to simply "w" as the mode when opening the file, and under Unix
          there is no penalty for doing so. gdImagePng does _not_ close
          the file; your code must do so.
          

... inside a function ...
[94]gdImagePtr im;
int black, white;
FILE *out;
/* Create the image */
im = [95]gdImageCreate(100, 100);
/* Allocate background */
white = [96]gdImageColorAllocate(im, 255, 255, 255);
/* Allocate drawing color */
black = [97]gdImageColorAllocate(im, 0, 0, 0);
/* Draw rectangle */
[98]gdImageRectangle(im, 0, 0, 99, 99, black);
/* Open output file in binary mode */
out = fopen("rect.png", "wb");
/* Write PNG */
gdImagePng(im, out);
/* Close file */
fclose(out);
/* Destroy image */
[99]gdImageDestroy(im);

   void* gdImagePngPtr(gdImagePtr im, int *size) _(FUNCTION)_
          Identical to gdImagePng except that it returns a pointer to a
          memory area with the PNG data. This memory must be freed by the
          caller when it is no longer needed. The 'size' parameter
          received the total size of the block of memory.
          
   gdImagePngToSink(gdImagePtr im, gdSinkPtr out) _(FUNCTION)_
          gdImagePngToSink is called to write a PNG to a data "sink"
          (destination) other than a file. Usage is very similar to the
          [100]gdImagePng function, except that the programmer provides a
          custom data sink.
          
          The programmer must write an output function which accepts a
          context pointer, a buffer, and a number of bytes to be written
          as arguments. This function must write the number of bytes
          requested and return that number, unless an error has occurred,
          in which case the function should return -1. The programmer
          then creates a [101]gdSink structure and sets the sink pointer
          to the output function and the context pointer to any value
          which is useful to the programmer.
          
          The example below implements [102]gdImagePng by creating a
          custom data source and invoking gdImagePngFromSink.
          

static int stdioSink(void *context, char *buffer, int len)
{
        return fwrite(buffer, 1, len, (FILE *) context);
}

void gdImagePng(gdImagePtr im, FILE *out)
{
        gdSink mySink;
        mySink.context = (void *) out;
        mySink.sink = stdioSink;
        gdImagePngToSink(im, &mySink);
}

   void gdImageGd(gdImagePtr im, FILE *out) _(FUNCTION)_
          gdImageGd outputs the specified image to the specified file in
          the [103]gd image format. The file must be open for writing.
          Under MSDOS and all versions of Windows, it is important to use
          "wb" as opposed to simply "w" as the mode when opening the
          file, and under Unix there is no penalty for doing so.
          gdImagePng does _not_ close the file; your code must do so.
          
          The gd image format is intended for fast reads and writes of
          images your program will need frequently to build other images.
          It is _not_ a compressed format, and is not intended for
          general use.
          

... inside a function ...
[104]gdImagePtr im;
int black, white;
FILE *out;
/* Create the image */
im = [105]gdImageCreate(100, 100);
/* Allocate background */
white = [106]gdImageColorAllocate(im, 255, 255, 255);
/* Allocate drawing color */
black = [107]gdImageColorAllocate(im, 0, 0, 0);
/* Draw rectangle */
[108]gdImageRectangle(im, 0, 0, 99, 99, black);
/* Open output file in binary mode */
out = fopen("rect.gd", "wb");
/* Write gd format file */
gdImageGd(im, out);
/* Close file */
fclose(out);
/* Destroy image */
[109]gdImageDestroy(im);

   void* gdImageGdPtr(gdImagePtr im, int *size) _(FUNCTION)_
          Identical to gdImageGd except that it returns a pointer to a
          memory area with the GD data. This memory must be freed by the
          caller when it is no longer needed. The 'size' parameter
          received the total size of the block of memory.
          
   void gdImageGd2(gdImagePtr im, FILE *out, int chunkSize, int fmt)
          _(FUNCTION)_
          gdImageGd2 outputs the specified image to the specified file in
          the [110]gd2 image format. The file must be open for writing.
          Under MSDOS and all versions of Windows, it is important to use
          "wb" as opposed to simply "w" as the mode when opening the
          file, and under Unix there is no penalty for doing so.
          gdImageGd2 does _not_ close the file; your code must do so.
          
          The gd2 image format is intended for fast reads and writes of
          parts of images. It is a compressed format, and well suited to
          retrieving smll sections of much larger images. The third and
          fourth parameters are the 'chunk size' and format
          resposectively.
          
          The file is stored as a series of compressed subimages, and the
          _Chunk Size_ determines the sub-image size - a value of zero
          causes the GD library to use the default.
          
          It is also possible to store GD2 files in an uncompressed
          format, in which case the fourth parameter should be
          GD2_FMT_RAW.
          

... inside a function ...
[111]gdImagePtr im;
int black, white;
FILE *out;
/* Create the image */
im = [112]gdImageCreate(100, 100);
/* Allocate background */
white = [113]gdImageColorAllocate(im, 255, 255, 255);
/* Allocate drawing color */
black = [114]gdImageColorAllocate(im, 0, 0, 0);
/* Draw rectangle */
[115]gdImageRectangle(im, 0, 0, 99, 99, black);
/* Open output file in binary mode */
out = fopen("rect.gd", "wb");
/* Write gd2 format file */
gdImageGd2(im, out, 0, GD2_FMT_COMPRESSED);
/* Close file */
fclose(out);
/* Destroy image */
[116]gdImageDestroy(im);

   void* gdImageGd2Ptr(gdImagePtr im, int chunkSize, int fmt, int *size)
          _(FUNCTION)_
          Identical to gdImageGd2 except that it returns a pointer to a
          memory area with the GD2 data. This memory must be freed by the
          caller when it is no longer needed. The 'size' parameter
          received the total size of the block of memory.
          
  Drawing Functions
  
   void gdImageSetPixel(gdImagePtr im, int x, int y, int color)
          _(FUNCTION)_
          gdImageSetPixel sets a pixel to a particular color index.
          Always use this function or one of the other drawing functions
          to access pixels; do not access the pixels of the [117]gdImage
          structure directly.
          

... inside a function ...
[118]gdImagePtr im;
int black;
int white;
im = [119]gdImageCreate(100, 100);
/* Background color (first allocated) */
black = [120]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color white (red, green and blue all maximum). */
white = [121]gdImageColorAllocate(im, 255, 255, 255);
/* Set a pixel near the center. */
gdImageSetPixel(im, 50, 50, white);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[122]gdImageDestroy(im);

   void gdImageLine(gdImagePtr im, int x1, int y1, int x2, int y2, int
          color) _(FUNCTION)_
          gdImageLine is used to draw a line between two endpoints (x1,y1
          and x2, y2). The line is drawn using the color index specified.
          Note that the color index can be an actual color returned by
          [123]gdImageColorAllocate or one of [124]gdStyled,
          [125]gdBrushed or [126]gdStyledBrushed.
          

... inside a function ...
[127]gdImagePtr im;
int black;
int white;
im = [128]gdImageCreate(100, 100);
/* Background color (first allocated) */
black = [129]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color white (red, green and blue all maximum). */
white = [130]gdImageColorAllocate(im, 255, 255, 255);
/* Draw a line from the upper left corner to the lower right corner. */
gdImageLine(im, 0, 0, 99, 99, white);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[131]gdImageDestroy(im);

   void gdImageDashedLine(gdImagePtr im, int x1, int y1, int x2, int y2,
          int color) _(FUNCTION)_
          gdImageDashedLine is provided _solely for backwards
          compatibility _with gd 1.0. New programs should draw dashed
          lines using the normal [132]gdImageLine function and the new
          [133]gdImageSetStyle function.
          
          gdImageDashedLine is used to draw a dashed line between two
          endpoints (x1,y1 and x2, y2). The line is drawn using the color
          index specified. The portions of the line that are not drawn
          are left transparent so the background is visible.
          

... inside a function ...
[134]gdImagePtr im;
int black;
int white;
im = [135]gdImageCreate(100, 100);
/* Background color (first allocated) */
black = [136]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color white (red, green and blue all maximum). */
white = [137]gdImageColorAllocate(im, 255, 255, 255);
/* Draw a dashed line from the upper left corner to the lower right corner. */
gdImageDashedLine(im, 0, 0, 99, 99);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[138]gdImageDestroy(im);

   void gdImagePolygon(gdImagePtr im, gdPointPtr points, int pointsTotal,
          int color) _(FUNCTION)_
          gdImagePolygon is used to draw a polygon with the verticies (at
          least 3) specified, using the color index specified. See also
          [139]gdImageFilledPolygon.
          

... inside a function ...
[140]gdImagePtr im;
int black;
int white;
/* Points of polygon */
[141]gdPoint points[3];
im = [142]gdImageCreate(100, 100);
/* Background color (first allocated) */
black = [143]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color white (red, green and blue all maximum). */
white = [144]gdImageColorAllocate(im, 255, 255, 255);
/* Draw a triangle. */
points[0].x = 50;
points[0].y = 0;
points[1].x = 99;
points[1].y = 99;
points[2].x = 0;
points[2].y = 99;
gdImagePolygon(im, points, 3, white);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[145]gdImageDestroy(im);

   void gdImageRectangle(gdImagePtr im, int x1, int y1, int x2, int y2,
          int color) _(FUNCTION)_
          gdImageRectangle is used to draw a rectangle with the two
          corners (upper left first, then lower right) specified, using
          the color index specified.
          

... inside a function ...
[146]gdImagePtr im;
int black;
int white;
im = [147]gdImageCreate(100, 100);
/* Background color (first allocated) */
black = [148]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color white (red, green and blue all maximum). */
white = [149]gdImageColorAllocate(im, 255, 255, 255);
/* Draw a rectangle occupying the central area. */
gdImageRectangle(im, 25, 25, 74, 74, white);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[150]gdImageDestroy(im);

   void gdImageFilledPolygon(gdImagePtr im, gdPointPtr points, int
          pointsTotal, int color) _(FUNCTION)_
          gdImageFilledPolygon is used to fill a polygon with the
          verticies (at least 3) specified, using the color index
          specified. See also [151]gdImagePolygon.
          

... inside a function ...
[152]gdImagePtr im;
int black;
int white;
int red;
/* Points of polygon */
[153]gdPoint points[3];
im = [154]gdImageCreate(100, 100);
/* Background color (first allocated) */
black = [155]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color white (red, green and blue all maximum). */
white = [156]gdImageColorAllocate(im, 255, 255, 255);
/* Allocate the color red. */
red = [157]gdImageColorAllocate(im, 255, 0, 0);
/* Draw a triangle. */
points[0].x = 50;
points[0].y = 0;
points[1].x = 99;
points[1].y = 99;
points[2].x = 0;
points[2].y = 99;
/* Paint it in white */
gdImageFilledPolygon(im, points, 3, white);
/* Outline it in red; must be done second */
[158]gdImagePolygon(im, points, 3, red);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[159]gdImageDestroy(im);

   void gdImageFilledRectangle(gdImagePtr im, int x1, int y1, int x2, int
          y2, int color) _(FUNCTION)_
          gdImageFilledRectangle is used to draw a solid rectangle with
          the two corners (upper left first, then lower right) specified,
          using the color index specified.
          

... inside a function ...
[160]gdImagePtr im;
int black;
int white;
im = [161]gdImageCreate(100, 100);
/* Background color (first allocated) */
black = [162]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color white (red, green and blue all maximum). */
white = [163]int gdImageColorAllocate(im, 255, 255, 255);
/* Draw a filled rectangle occupying the central area. */
gdImageFilledRectangle(im, 25, 25, 74, 74, white);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[164]gdImageDestroy(im);

   void gdImageArc(gdImagePtr im, int cx, int cy, int w, int h, int s,
          int e, int color) _(FUNCTION)_
          gdImageArc is used to draw a partial ellipse centered at the
          given point, with the specified width and height in pixels. The
          arc begins at the position in degrees specified by s and ends
          at the position specified by e. The arc is drawn in the color
          specified by the last argument. A circle can be drawn by
          beginning from 0 degrees and ending at 360 degrees, with width
          and height being equal. e must be greater than s. Values
          greater than 360 are interpreted modulo 360.
          

... inside a function ...
[165]gdImagePtr im;
int black;
int white;
im = [166]gdImageCreate(100, 50);
/* Background color (first allocated) */
black = [167]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color white (red, green and blue all maximum). */
white = [168]gdImageColorAllocate(im, 255, 255, 255);
/* Inscribe an ellipse in the image. */
gdImageArc(im, 50, 25, 98, 48, 0, 360, white);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[169]gdImageDestroy(im);

   void gdImageFillToBorder(gdImagePtr im, int x, int y, int border, int
          color) _(FUNCTION)_
          gdImageFillToBorder floods a portion of the image with the
          specified color, beginning at the specified point and stopping
          at the specified border color. For a way of flooding an area
          defined by the color of the starting point, see
          [170]gdImageFill.
          
          The border color _cannot_ be a special color such as
          [171]gdTiled; it must be a proper solid color. The fill color
          can be, however.
          
          Note that gdImageFillToBorder is recursive. It is not the most
          naive implementation possible, and the implementation is
          expected to improve, but there will always be degenerate cases
          in which the stack can become very deep. This can be a problem
          in MSDOS and MS Windows 3.1 environments. (Of course, in a Unix
          or Windows 95/98/NT environment with a proper stack, this is
          not a problem at all.)
          

... inside a function ...
[172]gdImagePtr im;
int black;
int white;
int red;
im = [173]gdImageCreate(100, 50);
/* Background color (first allocated) */
black = [174]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color white (red, green and blue all maximum). */
white = [175]gdImageColorAllocate(im, 255, 255, 255);
/* Allocate the color red. */
red = [176]gdImageColorAllocate(im, 255, 0, 0);
/* Inscribe an ellipse in the image. */
gdImageArc(im, 50, 25, 98, 48, 0, 360, white);
/* Flood-fill the ellipse. Fill color is red, border color is
        white (ellipse). */
gdImageFillToBorder(im, 50, 50, white, red);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[177]gdImageDestroy(im);

   void gdImageFill(gdImagePtr im, int x, int y, int color) _(FUNCTION)_
          gdImageFill floods a portion of the image with the specified
          color, beginning at the specified point and flooding the
          surrounding region of the same color as the starting point. For
          a way of flooding a region defined by a specific border color
          rather than by its interior color, see
          [178]gdImageFillToBorder.
          
          The fill color can be [179]gdTiled, resulting in a tile fill
          using another image as the tile. However, the tile image cannot
          be transparent. If the image you wish to fill with has a
          transparent color index, call [180]gdImageTransparent on the
          tile image and set the transparent color index to -1 to turn
          off its transparency.
          
          Note that gdImageFill is recursive. It is not the most naive
          implementation possible, and the implementation is expected to
          improve, but there will always be degenerate cases in which the
          stack can become very deep. This can be a problem in MSDOS and
          MS Windows environments. (Of course, in a Unix or Windows
          95/98/NT environment with a proper stack, this is not a problem
          at all.)
          

... inside a function ...
[181]gdImagePtr im;
int black;
int white;
int red;
im = [182]gdImageCreate(100, 50);
/* Background color (first allocated) */
black = [183]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color white (red, green and blue all maximum). */
white = [184]gdImageColorAllocate(im, 255, 255, 255);
/* Allocate the color red. */
red = [185]gdImageColorAllocate(im, 255, 0, 0);
/* Inscribe an ellipse in the image. */
gdImageArc(im, 50, 25, 98, 48, 0, 360, white);
/* Flood-fill the ellipse. Fill color is red, and will replace the
        black interior of the ellipse. */
gdImageFill(im, 50, 50, red);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[186]gdImageDestroy(im);

   void gdImageSetBrush(gdImagePtr im, gdImagePtr brush) _(FUNCTION)_
          A "brush" is an image used to draw wide, shaped strokes in
          another image. Just as a paintbrush is not a single point, a
          brush image need not be a single pixel. _Any_ gd image can be
          used as a brush, and by setting the transparent color index of
          the brush image with [187]gdImageColorTransparent, a brush of
          any shape can be created. All line-drawing functions, such as
          [188]gdImageLine and [189]gdImagePolygon, will use the current
          brush if the special "color" [190]gdBrushed or
          [191]gdStyledBrushed is used when calling them.
          
          gdImageSetBrush is used to specify the brush to be used in a
          particular image. You can set any image to be the brush. If the
          brush image does not have the same color map as the first
          image, any colors missing from the first image will be
          allocated. If not enough colors can be allocated, the closest
          colors already available will be used. This allows arbitrary
          PNGs to be used as brush images. It also means, however, that
          you should not set a brush unless you will actually use it; if
          you set a rapid succession of different brush images, you can
          quickly fill your color map, and the results will not be
          optimal.
          
          You need not take any special action when you are finished with
          a brush. As for any other image, if you will not be using the
          brush image for any further purpose, you should call
          [192]gdImageDestroy. You must not use the color [193]gdBrushed
          if the current brush has been destroyed; you can of course set
          a new brush to replace it.
          

... inside a function ...
[194]gdImagePtr im, brush;
FILE *in;
int black;
im = [195]gdImageCreate(100, 100);
/* Open the brush PNG. For best results, portions of the
        brush that should be transparent (ie, not part of the
        brush shape) should have the transparent color index. */
in = fopen("star.png", "rb");
brush = [196]gdImageCreateFromPng(in);
/* Background color (first allocated) */
black = [197]gdImageColorAllocate(im, 0, 0, 0);
gdImageSetBrush(im, brush);
/* Draw a line from the upper left corner to the lower right corner
        using the brush. */
[198]gdImageLine(im, 0, 0, 99, 99, [199]gdBrushed);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[200]gdImageDestroy(im);
/* Destroy the brush image */
[201]gdImageDestroy(brush);

   void gdImageSetTile(gdImagePtr im, gdImagePtr tile) _(FUNCTION)_
          A "tile" is an image used to fill an area with a repeated
          pattern. _Any_ gd image can be used as a tile, and by setting
          the transparent color index of the tile image with
          [202]gdImageColorTransparent, a tile that allows certain parts
          of the underlying area to shine through can be created. All
          region-filling functions, such as [203]gdImageFill and
          [204]gdImageFilledPolygon, will use the current tile if the
          special "color" [205]gdTiled is used when calling them.
          
          gdImageSetTile is used to specify the tile to be used in a
          particular image. You can set any image to be the tile. If the
          tile image does not have the same color map as the first image,
          any colors missing from the first image will be allocated. If
          not enough colors can be allocated, the closest colors already
          available will be used. This allows arbitrary PNGs to be used
          as tile images. It also means, however, that you should not set
          a tile unless you will actually use it; if you set a rapid
          succession of different tile images, you can quickly fill your
          color map, and the results will not be optimal.
          
          You need not take any special action when you are finished with
          a tile. As for any other image, if you will not be using the
          tile image for any further purpose, you should call
          [206]gdImageDestroy. You must not use the color [207]gdTiled if
          the current tile has been destroyed; you can of course set a
          new tile to replace it.
          

... inside a function ...
[208]gdImagePtr im, tile;
FILE *in;
int black;
im = [209]gdImageCreate(100, 100);
/* Open the tile PNG. For best results, portions of the
        tile that should be transparent (ie, allowing the
        background to shine through) should have the transparent
        color index. */
in = fopen("star.png", "rb");
tile = [210]gdImageCreateFromPng(in);
/* Background color (first allocated) */
black = [211]gdImageColorAllocate(im, 0, 0, 0);
gdImageSetTile(im, tile);
/* Fill an area using the tile. */
[212]gdImageFilledRectangle(im, 25, 25, 75, 75, [213]gdTiled);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[214]gdImageDestroy(im);
/* Destroy the tile image */
[215]gdImageDestroy(tile);

   void gdImageSetStyle(gdImagePtr im, int *style, int styleLength)
          _(FUNCTION)_
          It is often desirable to draw dashed lines, dotted lines, and
          other variations on a broken line. gdImageSetStyle can be used
          to set any desired series of colors, including a special color
          that leaves the background intact, to be repeated during the
          drawing of a line.
          
          To use gdImageSetStyle, create an array of integers and assign
          them the desired series of color values to be repeated. You can
          assign the special color value [216]gdTransparent to indicate
          that the existing color should be left unchanged for that
          particular pixel (allowing a dashed line to be attractively
          drawn over an existing image).
          
          Then, to draw a line using the style, use the normal
          [217]gdImageLine function with the special color value
          [218]gdStyled.
          
          As of [219]version 1.1.1, the style array is copied when you
          set the style, so you need not be concerned with keeping the
          array around indefinitely. This should not break existing code
          that assumes styles are not copied.
          
          You can also combine styles and brushes to draw the brush image
          at intervals instead of in a continuous stroke. When creating a
          style for use with a brush, the style values are interpreted
          differently: zero (0) indicates pixels at which the brush
          should not be drawn, while one (1) indicates pixels at which
          the brush should be drawn. To draw a styled, brushed line, you
          must use the special color value [220]gdStyledBrushed. For an
          example of this feature in use, see gddemo.c (provided in the
          distribution).
          

[221]gdImagePtr im;
int styleDotted[2], styleDashed[6];
FILE *in;
int black;
int red;
im = [222]gdImageCreate(100, 100);
/* Background color (first allocated) */
black = [223]gdImageColorAllocate(im, 0, 0, 0);
red = [224]gdImageColorAllocate(im, 255, 0, 0);
/* Set up dotted style. Leave every other pixel alone. */
styleDotted[0] = red;
styleDotted[1] = gdTransparent;
/* Set up dashed style. Three on, three off. */
styleDashed[0] = red;
styleDashed[1] = red;
styleDashed[2] = red;
styleDashed[3] = gdTransparent;
styleDashed[4] = gdTransparent;
styleDashed[5] = gdTransparent;
/* Set dotted style. Note that we have to specify how many pixels are
        in the style! */
gdImageSetStyle(im, styleDotted, 2);
/* Draw a line from the upper left corner to the lower right corner. */
[225]gdImageLine(im, 0, 0, 99, 99, [226]gdStyled);
/* Now the dashed line. */
gdImageSetStyle(im, styleDashed, 6);
[227]gdImageLine(im, 0, 99, 0, 99, [228]gdStyled);

/* ... Do something with the image, such as saving it to a file ... */

/* Destroy it */
[229]gdImageDestroy(im);

  Query Functions
  
        int gdImageBlue(gdImagePtr im, int color) _(MACRO)_
                gdImageBlue is a macro which returns the blue component
                of the specified color index. Use this macro rather than
                accessing the structure members directly.
                
        int gdImageGetPixel(gdImagePtr im, int x, int y) _(FUNCTION)_
                gdImageGetPixel() retrieves the color index of a
                particular pixel. Always use this function to query
                pixels; do not access the pixels of the [230]gdImage
                structure directly.
                

... inside a function ...
FILE *in;
gdImagePtr im;
int c;
in = fopen("mypng.png", "rb");
im = [231]gdImageCreateFromPng(in);
fclose(in);
c = gdImageGetPixel(im, gdImageSX(im) / 2, gdImageSY(im) / 2);
printf("The value of the center pixel is %d; RGB values are %d,%d,%d\n",
        c, im->red[c], im->green[c], im->blue[c]);
[232]gdImageDestroy(im);

        int gdImageBoundsSafe(gdImagePtr im, int x, int y) _(FUNCTION)_
                gdImageBoundsSafe returns true (1) if the specified point
                is within the bounds of the image, false (0) if not. This
                function is intended primarily for use by those who wish
                to add functions to gd. All of the gd drawing functions
                already clip safely to the edges of the image.
                

... inside a function ...
[233]gdImagePtr im;
int black;
int white;
im = [234]gdImageCreate(100, 100);
if (gdImageBoundsSafe(im, 50, 50)) {
        printf("50, 50 is within the image bounds\n");
} else {
        printf("50, 50 is outside the image bounds\n");
}
[235]gdImageDestroy(im);

        int gdImageGreen(gdImagePtr im, int color) _(MACRO)_
                gdImageGreen is a macro which returns the green component
                of the specified color index. Use this macro rather than
                accessing the structure members directly.
                
        int gdImageRed(gdImagePtr im, int color) _(MACRO)_
                gdImageRed is a macro which returns the red component of
                the specified color index. Use this macro rather than
                accessing the structure members directly.
                
        int gdImageSX(gdImagePtr im) _(MACRO)_
                gdImageSX is a macro which returns the width of the image
                in pixels. Use this macro rather than accessing the
                structure members directly.
                
        int gdImageSY(gdImagePtr im) _(MACRO)_
                gdImageSY is a macro which returns the height of the
                image in pixels. Use this macro rather than accessing the
                structure members directly.
                
  Fonts and text-handling functions
  
        void gdImageChar(gdImagePtr im, gdFontPtr font, int x, int y, int
                c, int color) _(FUNCTION)_
                gdImageChar is used to draw single characters on the
                image. (To draw multiple characters, use
                [236]gdImageString or [237]gdImageString16.) The second
                argument is a pointer to a font definition structure;
                five fonts are provided with gd, gdFontTiny, gdFontSmall,
                gdFontMediumBold, gdFontLarge, and gdFontGiant. You must
                include the files "gdfontt.h", "gdfonts.h", "gdfontmb.h",
                "gdfontl.h" and "gdfontg.h" respectively and (if you are
                not using a library-based approach) link with the
                corresponding .c files to use the provided fonts. The
                character specified by the fifth argument is drawn from
                left to right in the specified color. (See
                [238]gdImageCharUp for a way of drawing vertical text.)
                Pixels not set by a particular character retain their
                previous color.
                

#include "gd.h"
#include "gdfontl.h"
... inside a function ...
[239]gdImagePtr im;
int black;
int white;
im = [240]gdImageCreate(100, 100);
/* Background color (first allocated) */
black = [241]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color white (red, green and blue all maximum). */
white = [242]gdImageColorAllocate(im, 255, 255, 255);
/* Draw a character. */
gdImageChar(im, gdFontLarge, 0, 0, 'Q', white);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[243]gdImageDestroy(im);

        void gdImageCharUp(gdImagePtr im, gdFontPtr font, int x, int y,
                int c, int color) _(FUNCTION)_
                gdImageCharUp is used to draw single characters on the
                image, rotated 90 degrees. (To draw multiple characters,
                use [244]gdImageStringUp or [245]gdImageStringUp16.) The
                second argument is a pointer to a font definition
                structure; five fonts are provided with gd, gdFontTiny,
                gdFontSmall, gdFontMediumBold, gdFontLarge, and
                gdFontGiant. You must include the files "gdfontt.h",
                "gdfonts.h", "gdfontmb.h", "gdfontl.h" and "gdfontg.h"
                respectively and (if you are not using a library-based
                approach) link with the corresponding .c files to use the
                provided fonts. The character specified by the fifth
                argument is drawn from bottom to top, rotated at a
                90-degree angle, in the specified color. (See
                [246]gdImageChar for a way of drawing horizontal text.)
                Pixels not set by a particular character retain their
                previous color.
                

#include "gd.h"
#include "gdfontl.h"
... inside a function ...
[247]gdImagePtr im;
int black;
int white;
im = [248]gdImageCreate(100, 100);
/* Background color (first allocated) */
black = [249]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color white (red, green and blue all maximum). */
white = [250]gdImageColorAllocate(im, 255, 255, 255);
/* Draw a character upwards so it rests against the top of the image. */
gdImageCharUp(im, gdFontLarge,
        0, gdFontLarge->h, 'Q', white);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[251]gdImageDestroy(im);

        void gdImageString(gdImagePtr im, gdFontPtr font, int x, int y,
                unsigned char *s, int color) _(FUNCTION)_
                gdImageString is used to draw multiple characters on the
                image. (To draw single characters, use [252]gdImageChar.)
                The second argument is a pointer to a font definition
                structure; five fonts are provided with gd, gdFontTiny,
                gdFontSmall, gdFontMediumBold, gdFontLarge, and
                gdFontGiant. You must include the files "gdfontt.h",
                "gdfonts.h", "gdfontmb.h", "gdfontl.h" and "gdfontg.h"
                respectively and (if you are not using a library-based
                approach) link with the corresponding .c files to use the
                provided fonts. The null-terminated C string specified by
                the fifth argument is drawn from left to right in the
                specified color. (See [253]gdImageStringUp for a way of
                drawing vertical text.) Pixels not set by a particular
                character retain their previous color.
                

#include "gd.h"
#include "gdfontl.h"
#include <string.h>
... inside a function ...
[254]gdImagePtr im;
int black;
int white;
/* String to draw. */
char *s = "Hello.";
im = [255]gdImageCreate(100, 100);
/* Background color (first allocated) */
black = [256]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color white (red, green and blue all maximum). */
white = [257]gdImageColorAllocate(im, 255, 255, 255);
/* Draw a centered string. */
gdImageString(im, gdFontLarge,
        im->w / 2 - (strlen(s) * gdFontLarge->w / 2),
        im->h / 2 - gdFontLarge->h / 2,
        s, white);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[258]gdImageDestroy(im);

        void gdImageString16(gdImagePtr im, gdFontPtr font, int x, int y,
                unsigned short *s, int color) _(FUNCTION)_
                gdImageString is used to draw multiple 16-bit characters
                on the image. (To draw single characters, use
                [259]gdImageChar.) The second argument is a pointer to a
                font definition structure; five fonts are provided with
                gd, gdFontTiny, gdFontSmall, gdFontMediumBold,
                gdFontLarge, and gdFontGiant. You must include the files
                "gdfontt.h", "gdfonts.h", "gdfontmb.h", "gdfontl.h" and
                "gdfontg.h" respectively and (if you are not using a
                library-based approach) link with the corresponding .c
                files to use the provided fonts. The null-terminated
                string of characters represented as 16-bit unsigned short
                integers specified by the fifth argument is drawn from
                left to right in the specified color. (See
                [260]gdImageStringUp16 for a way of drawing vertical
                text.) Pixels not set by a particular character retain
                their previous color.
                
                This function was added in gd1.3 to provide a means of
                rendering fonts with more than 256 characters for those
                who have them. A more frequently used routine is
                [261]gdImageString.
                
        void gdImageStringUp(gdImagePtr im, gdFontPtr font, int x, int y,
                unsigned char *s, int color) _(FUNCTION)_
                gdImageStringUp is used to draw multiple characters on
                the image, rotated 90 degrees. (To draw single
                characters, use [262]gdImageCharUp.) The second argument
                is a pointer to a font definition structure; five fonts
                are provided with gd, gdFontTiny, gdFontSmall,
                gdFontMediumBold, gdFontLarge, and gdFontGiant. You must
                include the files "gdfontt.h", "gdfonts.h", "gdfontmb.h",
                "gdfontl.h" and "gdfontg.h" respectively and (if you are
                not using a library-based approach) link with the
                corresponding .c files to use the provided fonts.The
                null-terminated C string specified by the fifth argument
                is drawn from bottom to top (rotated 90 degrees) in the
                specified color. (See [263]gdImageString for a way of
                drawing horizontal text.) Pixels not set by a particular
                character retain their previous color.
                

#include "gd.h"
#include "gdfontl.h"
#include <string.h>
... inside a function ...
[264]gdImagePtr im;
int black;
int white;
/* String to draw. */
char *s = "Hello.";
im = [265]gdImageCreate(100, 100);
/* Background color (first allocated) */
black = [266]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color white (red, green and blue all maximum). */
white = [267]gdImageColorAllocate(im, 255, 255, 255);
/* Draw a centered string going upwards. Axes are reversed,
        and Y axis is decreasing as the string is drawn. */
gdImageStringUp(im, gdFontLarge,
        im->w / 2 - gdFontLarge->h / 2,
        im->h / 2 + (strlen(s) * gdFontLarge->w / 2),
        s, white);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[268]gdImageDestroy(im);

        void gdImageStringUp16(gdImagePtr im, gdFontPtr font, int x, int
                y, unsigned short *s, int color) _(FUNCTION)_
                gdImageString is used to draw multiple 16-bit characters
                vertically on the image. (To draw single characters, use
                [269]gdImageChar.) The second argument is a pointer to a
                font definition structure; five fonts are provided with
                gd, gdFontTiny, gdFontSmall, gdFontMediumBold,
                gdFontLarge, and gdFontGiant. You must include the files
                "gdfontt.h", "gdfonts.h", "gdfontmb.h", "gdfontl.h" and
                "gdfontg.h" respectively and (if you are not using a
                library-based approach) link with the corresponding .c
                files to use the provided fonts. The null-terminated
                string of characters represented as 16-bit unsigned short
                integers specified by the fifth argument is drawn from
                bottom to top in the specified color. (See
                [270]gdImageStringUp16 for a way of drawing horizontal
                text.) Pixels not set by a particular character retain
                their previous color.
                
                This function was added in gd1.3 to provide a means of
                rendering fonts with more than 256 characters for those
                who have them. A more frequently used routine is
                [271]gdImageStringUp.
                
  Color-handling functions
  
        int gdImageColorAllocate(gdImagePtr im, int r, int g, int b)
                _(FUNCTION)_
                gdImageColorAllocate finds the first available color
                index in the image specified, sets its RGB values to
                those requested (255 is the maximum for each), and
                returns the index of the new color table entry. When
                creating a new image, the first time you invoke this
                function, you are setting the background color for that
                image.
                
                In the event that all [272]gdMaxColors colors (256) have
                already been allocated, gdImageColorAllocate will return
                -1 to indicate failure. (This is not uncommon when
                working with existing PNG files that already use 256
                colors.) Note that gdImageColorAllocate does not check
                for existing colors that match your request; see
                [273]gdImageColorExact and [274]gdImageColorClosest for
                ways to locate existing colors that approximate the color
                desired in situations where a new color is not available.
                

... inside a function ...
[275]gdImagePtr im;
int black;
int red;
im = [276]gdImageCreate(100, 100);
/* Background color (first allocated) */
black = [277]gdImageColorAllocate(im, 0, 0, 0);
/* Allocate the color red. */
red = [278]gdImageColorAllocate(im, 255, 0, 0);
/* Draw a dashed line from the upper left corner to the lower right corner. */
gdImageDashedLine(im, 0, 0, 99, 99, red);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[279]gdImageDestroy(im);

        int gdImageColorClosest(gdImagePtr im, int r, int g, int b)
                _(FUNCTION)_
                gdImageColorClosest searches the colors which have been
                defined thus far in the image specified and returns the
                index of the color with RGB values closest to those of
                the request. (Closeness is determined by Euclidian
                distance, which is used to determine the distance in
                three-dimensional color space between colors.)
                
                If no colors have yet been allocated in the image,
                gdImageColorClosest returns -1.
                
                This function is most useful as a backup method for
                choosing a drawing color when an image already contains
                [280]gdMaxColors (256) colors and no more can be
                allocated. (This is not uncommon when working with
                existing PNG files that already use many colors.) See
                [281]gdImageColorExact for a method of locating exact
                matches only.
                

... inside a function ...
[282]gdImagePtr im;
FILE *in;
int red;
/* Let's suppose that photo.png is a scanned photograph with
        many colors. */
in = fopen("photo.png", "rb");
im = [283]gdImageCreateFromPng(in);
fclose(in);
/* Try to allocate red directly */
red = [284]gdImageColorAllocate(im, 255, 0, 0);
/* If we fail to allocate red... */
if (red == (-1)) {
        /* Find the _closest_ color instead. */
        red = gdImageColorClosest(im, 255, 0, 0);
}
/* Draw a dashed line from the upper left corner to the lower right corner */
gdImageDashedLine(im, 0, 0, 99, 99, red);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[285]gdImageDestroy(im);

        int gdImageColorExact(gdImagePtr im, int r, int g, int b)
                _(FUNCTION)_
                gdImageColorExact searches the colors which have been
                defined thus far in the image specified and returns the
                index of the first color with RGB values which exactly
                match those of the request. If no allocated color matches
                the request precisely, gdImageColorExact returns -1. See
                [286]gdImageColorClosest for a way to find the color
                closest to the color requested.
                

... inside a function ...
[287]gdImagePtr im;
int red;
in = fopen("photo.png", "rb");
im = [288]gdImageCreateFromPng(in);
fclose(in);
/* The image may already contain red; if it does, we'll save a slot
        in the color table by using that color. */
/* Try to allocate red directly */
red = gdImageColorExact(im, 255, 0, 0);
/* If red isn't already present... */
if (red == (-1)) {
        /* Second best: try to allocate it directly. */
        red = [289]gdImageColorAllocate(im, 255, 0, 0);
        /* Out of colors, so find the _closest_ color instead. */
        red = gdImageColorClosest(im, 255, 0, 0);
}
/* Draw a dashed line from the upper left corner to the lower right corner */
gdImageDashedLine(im, 0, 0, 99, 99, red);
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[290]gdImageDestroy(im);

        int gdImageColorsTotal(gdImagePtr im) _(MACRO)_
                gdImageColorsTotal is a macro which returns the number of
                colors currently allocated in the image. Use this macro
                to obtain this information; do not access the structure
                directly.
                
        int gdImageColorRed(gdImagePtr im, int c) _(MACRO)_
                gdImageColorRed is a macro which returns the red portion
                of the specified color in the image. Use this macro to
                obtain this information; do not access the structure
                directly.
                
        int gdImageColorGreen(gdImagePtr im, int c) _(MACRO)_
                gdImageColorGreen is a macro which returns the green
                portion of the specified color in the image. Use this
                macro to obtain this information; do not access the
                structure directly.
                
        int gdImageColorBlue(gdImagePtr im, int c) _(MACRO)_
                gdImageColorBlue is a macro which returns the green
                portion of the specified color in the image. Use this
                macro to obtain this information; do not access the
                structure directly.
                
        int gdImageGetInterlaced(gdImagePtr im) _(MACRO)_
                gdImageGetInterlaced is a macro which returns true (1) if
                the image is interlaced, false (0) if not. Use this macro
                to obtain this information; do not access the structure
                directly. See gdImageInterlace for a means of interlacing
                images.
                
        int gdImageGetTransparent(gdImagePtr im) _(MACRO)_
                gdImageGetTransparent is a macro which returns the
                current transparent color index in the image. If there is
                no transparent color, gdImageGetTransparent returns -1.
                Use this macro to obtain this information; do not access
                the structure directly.
                
        void gdImageColorDeallocate(gdImagePtr im, int color) _(FUNCTION)_
                
                gdImageColorDeallocate marks the specified color as being
                available for reuse. It does not attempt to determine
                whether the color index is still in use in the image.
                After a call to this function, the next call to
                [291]gdImageColorAllocate for the same image will set new
                RGB values for that color index, changing the color of
                any pixels which have that index as a result. If multiple
                calls to gdImageColorDeallocate are made consecutively,
                the lowest-numbered index among them will be reused by
                the next [292]gdImageColorAllocate call.
                

... inside a function ...
[293]gdImagePtr im;
int red, blue;
in = fopen("photo.png", "rb");
im = [294]gdImageCreateFromPng(in);
fclose(in);
/* Look for red in the color table. */
red = gdImageColorExact(im, 255, 0, 0);
/* If red is present... */
if (red != (-1)) {
        /* Deallocate it. */
        gdImageColorDeallocate(im, red);
        /* Allocate blue, reusing slot in table.
                Existing red pixels will change color. */
        blue = gdImageColorAllocate(im, 0, 0, 255);
}
/* ... Do something with the image, such as saving it to a file... */
/* Destroy it */
[295]gdImageDestroy(im);

        void gdImageColorTransparent(gdImagePtr im, int color)
                _(FUNCTION)_
                gdImageColorTransparent sets the transparent color index
                for the specified image to the specified index. To
                indicate that there should be _no_ transparent color,
                invoke gdImageColorTransparent with a color index of -1.
                
                The color index used should be an index allocated by
                [296]gdImageColorAllocate, whether explicitly invoked by
                your code or implicitly invoked by loading an image. In
                order to ensure that your image has a reasonable
                appearance when viewed by users who do not have
                transparent background capabilities, be sure to give
                reasonable RGB values to the color you allocate for use
                as a transparent color, _even though it will be
                transparent on systems that support transparency_.
                

... inside a function ...
[297]gdImagePtr im;
int black;
FILE *in, *out;
in = fopen("photo.png", "rb");
im = [298]gdImageCreateFromPng(in);
fclose(in);
/* Look for black in the color table and make it transparent. */
black = [299]gdImageColorExact(im, 0, 0, 0);
/* If black is present... */
if (black != (-1)) {
        /* Make it transparent */
        gdImageColorTransparent(im, black);
}
/* Save the newly-transparent image back to the file */
out = fopen("photo.png", "wb");
[300]gdImagePng(im, out);
fclose(out);
/* Destroy it */
[301]gdImageDestroy(im);

  Copying and resizing functions
  
        void gdImageCopy(gdImagePtr dst, gdImagePtr src, int dstX, int
                dstY, int srcX, int srcY, int w, int h) _(FUNCTION)_
                gdImageCopy is used to copy a rectangular portion of one
                image to another image. (For a way of stretching or
                shrinking the image in the process, see
                [302]gdImageCopyResized.)
                
                The dst argument is the destination image to which the
                region will be copied. The src argument is the source
                image from which the region is copied. The dstX and dstY
                arguments specify the point in the destination image to
                which the region will be copied. The srcX and srcY
                arguments specify the upper left corner of the region in
                the source image. The w and h arguments specify the width
                and height of the region.
                
                When you copy a region from one location in an image to
                another location in the same image, gdImageCopy will
                perform as expected unless the regions overlap, in which
                case the result is unpredictable.
                
                _Important note on copying between images:_ since
                different images do not necessarily have the same color
                tables, pixels are not simply set to the same color index
                values to copy them. gdImageCopy will attempt to find an
                identical RGB value in the destination image for each
                pixel in the copied portion of the source image by
                invoking [303]gdImageColorExact. If such a value is not
                found, gdImageCopy will attempt to allocate colors as
                needed using [304]gdImageColorAllocate. If both of these
                methods fail, gdImageCopy will invoke
                [305]gdImageColorClosest to find the color in the
                destination image which most closely approximates the
                color of the pixel being copied.
                

... Inside a function ...
[306]gdImagePtr im_in;
[307]gdImagePtr im_out;
int x, y;
FILE *in;
FILE *out;
/* Load a small png to tile the larger one with */
in = fopen("small.png", "rb");
im_in = [308]gdImageCreateFromPng(in);
fclose(in);
/* Make the output image four times as large on both axes */
im_out = [309]gdImageCreate(im_in->sx * 4, im_in->sy * 4);
/* Now tile the larger image using the smaller one */
for (y = 0; (y < 4); y++) {
        for (x = 0; (x < 4); x++) {
                gdImageCopy(im_out, im_in,
                        x * im_in->sx, y * im_in->sy,
                        0, 0,
                        im_in->sx, im_in->sy);
        }
}
out = fopen("tiled.png", "wb");
[310]gdImagePng(im_out, out);
fclose(out);
[311]gdImageDestroy(im_in);
[312]gdImageDestroy(im_out);

        void gdImageCopyResized(gdImagePtr dst, gdImagePtr src, int dstX,
                int dstY, int srcX, int srcY, int destW, int destH, int
                srcW, int srcH) _(FUNCTION)_
                gdImageCopyResized is used to copy a rectangular portion
                of one image to another image. The X and Y dimensions of
                the original region and the destination region can vary,
                resulting in stretching or shrinking of the region as
                appropriate. (For a simpler version of this function
                which does not deal with resizing, see [313]gdImageCopy.)
                
                The dst argument is the destination image to which the
                region will be copied. The src argument is the source
                image from which the region is copied. The dstX and dstY
                arguments specify the point in the destination image to
                which the region will be copied. The srcX and srcY
                arguments specify the upper left corner of the region in
                the source image. The dstW and dstH arguments specify the
                width and height of the destination region. The srcW and
                srcH arguments specify the width and height of the source
                region and can differ from the destination size, allowing
                a region to be scaled during the copying process.
                
                When you copy a region from one location in an image to
                another location in the same image, gdImageCopy will
                perform as expected unless the regions overlap, in which
                case the result is unpredictable. If this presents a
                problem, create a scratch image in which to keep
                intermediate results.
                
                _Important note on copying between images:_ since images
                do not necessarily have the same color tables, pixels are
                not simply set to the same color index values to copy
                them. gdImageCopy will attempt to find an identical RGB
                value in the destination image for each pixel in the
                copied portion of the source image by invoking
                [314]gdImageColorExact. If such a value is not found,
                gdImageCopy will attempt to allocate colors as needed
                using [315]gdImageColorAllocate. If both of these methods
                fail, gdImageCopy will invoke [316]gdImageColorClosest to
                find the color in the destination image which most
                closely approximates the color of the pixel being copied.
                

... Inside a function ...
[317]gdImagePtr im_in;
[318]gdImagePtr im_out;
int x, y;
FILE *in;
FILE *out;
/* Load a small png to expand in the larger one */
in = fopen("small.png", "rb");
im_in = [319]gdImageCreateFromPng(in);
fclose(in);
/* Make the output image four times as large on both axes */
im_out = [320]gdImageCreate(im_in->sx * 4, im_in->sy * 4);
/* Now copy the smaller image, but four times larger */
gdImageCopyResized(im_out, im_in, 0, 0, 0, 0,
        im_out->sx, im_out->sy,
        im_in->sx, im_in->sy);
out = fopen("large.png", "wb");
[321]gdImagePng(im_out, out);
fclose(out);
[322]gdImageDestroy(im_in);
[323]gdImageDestroy(im_out);

        void gdImageCopyMerge(gdImagePtr dst, gdImagePtr src, int dstX,
                int dstY, int srcX, int srcY, int w, int h, int pct)
                _(FUNCTION)_
                gdImageCopyMerge is almost identical to [324]gdImageCopy,
                except that it 'merges' the two images by an amount
                specified in the last parameter. If the last parameter is
                100, then it will function identically to gdImageCopy -
                the source image replaces the pixels in the destination.
                
                If, however, the _pct_ parameter is less than 100, then
                the two images are merged. With pct = 0, no action is
                taken.
                
                This feature is most useful to 'highlight' sections of an
                image by merging a solid color with pct = 50:
                

... Inside a function ...
gdImageCopyMerge(im_out, im_in, 100, 200, 0, 0, 30, 50, 50);

        void gdImageCopyMergeGray(gdImagePtr dst, gdImagePtr src, int
                dstX, int dstY, int srcX, int srcY, int w, int h, int
                pct) _(FUNCTION)_
                gdImageCopyMergeGray is almost identical to
                [325]gdImageCopyMerge, except that when merging images it
                preserves the hue of the source by converting the
                destination pixels to grey scale before the copy
                operation.
                

... Inside a function ...
gdImageCopyMergeGray(im_out, im_in, 100, 200, 0, 0, 30, 50, 50);

        void gdImagePaletteCopy(gdImagePtr dst, gdImagePtr src)
                _(FUNCTION)_
                Copies a palette from one image to another, doing it's
                best to match the colors in the target image to the
                colors in the source palette.
                
  Miscellaneous Functions
  
              gdImageInterlace(gdImagePtr im, int interlace) _(FUNCTION)_
                      
                      gdImageInterlace is used to determine whether an
                      image should be stored in a linear fashion, in
                      which lines will appear on the display from first
                      to last, or in an interlaced fashion, in which the
                      image will "fade in" over several passes. By
                      default, images are not interlaced.
                      
                      A nonzero value for the interlace argument turns on
                      interlace; a zero value turns it off. Note that
                      interlace has no effect on other functions, and has
                      no meaning unless you save the image in PNG format;
                      the gd and xbm formats do not support interlace.
                      
                      When a PNG is loaded with [326]gdImageCreateFromPng
                      , interlace will be set according to the setting in
                      the PNG file.
                      
                      Note that many PNG viewers and web browsers do _not_
                      support interlace. However, the interlaced PNG
                      should still display; it will simply appear all at
                      once, just as other images do.
                      

gdImagePtr im;
FILE *out;
/* ... Create or load the image... */

/* Now turn on interlace */
gdImageInterlace(im, 1);
/* And open an output file */
out = fopen("test.png", "wb");
/* And save the image */
[327]gdImagePng(im, out);
fclose(out);
[328]gdImageDestroy(im);

  Constants
  
              gdBrushed _(CONSTANT)_
                      Used in place of a color when invoking a
                      line-drawing function such as [329]gdImageLine or
                      [330]gdImageRectangle. When gdBrushed is used as
                      the color, the brush image set with
                      [331]gdImageSetBrush is drawn in place of each
                      pixel of the line (the brush is usually larger than
                      one pixel, creating the effect of a wide
                      paintbrush). See also [332]gdStyledBrushed for a
                      way to draw broken lines with a series of distinct
                      copies of an image.
                      
              gdMaxColors_(CONSTANT)_
                      The constant 256. This is the maximum number of
                      colors in a PNG file according to the PNG standard,
                      and is also the maximum number of colors in a gd
                      image.
                      
              gdStyled _(CONSTANT)_
                      Used in place of a color when invoking a
                      line-drawing function such as [333]gdImageLine or
                      [334]gdImageRectangle. When gdStyled is used as the
                      color, the colors of the pixels are drawn
                      successively from the style that has been set with
                      [335]gdImageSetStyle. If the color of a pixel is
                      equal to [336]gdTransparent, that pixel is not
                      altered. (This mechanism is completely unrelated to
                      the "transparent color" of the image itself; see
                      [337]gdImageColorTransparent
                      gdImageColorTransparent for that mechanism.) See
                      also gdStyledBrushed.
                      
              gdStyledBrushed _(CONSTANT)_
                      Used in place of a color when invoking a
                      line-drawing function such as [338]gdImageLine or
                      [339]gdImageRectangle. When gdStyledBrushed is used
                      as the color, the brush image set with
                      [340]gdImageSetBrush is drawn at each pixel of the
                      line, providing that the style set with
                      [341]gdImageSetStyle contains a nonzero value (OR
                      gdTransparent, which does not equal zero but is
                      supported for consistency) for the current pixel.
                      (Pixels are drawn successively from the style as
                      the line is drawn, returning to the beginning when
                      the available pixels in the style are exhausted.)
                      Note that this differs from the behavior of
                      [342]gdStyled, in which the values in the style are
                      used as actual pixel colors, except for
                      gdTransparent.
                      
              gdDashSize _(CONSTANT)_
                      The length of a dash in a dashed line. Defined to
                      be 4 for backwards compatibility with programs that
                      use gdImageDashedLine. New programs should use
                      gdImageSetStyle and call the standard gdImageLine
                      function with the special "color" gdStyled or
                      gdStyledBrushed.
                      
              gdTiled _(CONSTANT)_
                      Used in place of a normal color in
                      [343]gdImageFilledRectangle,
                      [344]gdImageFilledPolygon, [345]gdImageFill, and
                      [346]gdImageFillToBorder. gdTiled selects a pixel
                      from the tile image set with [347]gdImageSetTile in
                      such a way as to ensure that the filled area will
                      be tiled with copies of the tile image. See the
                      discussions of [348]gdImageFill and
                      [349]gdImageFillToBorder for special restrictions
                      regarding those functions.
                      
              gdTransparent _(CONSTANT)_
                      Used in place of a normal color in a style to be
                      set with [350]gdImageSetStyle. gdTransparent is
                      _not_ the transparent color index of the image; for
                      that functionality please see
                      [351]gdImageColorTransparent.
                      
  About the additional .gd image file format
  
                      In addition to reading and writing the PNG format
                      and reading the X Bitmap format, gd has the
                      capability to read and write its own ".gd" format.
                      This format is _not_ intended for general purpose
                      use and should never be used to distribute images.
                      It is not a compressed format. Its purpose is
                      solely to allow very fast loading of images your
                      program needs often in order to build other images
                      for output. If you are experiencing performance
                      problems when loading large, fixed PNG images your
                      program needs to produce its output images, you may
                      wish to examine the functions
                      [352]gdImageCreateFromGd and [353]gdImageGd, which
                      read and write .gd format images.
                      
                      The program "pngtogd.c" is provided as a simple way
                      of converting .png files to .gd format. I emphasize
                      again that you will not need to use this format
                      unless you have a need for high-speed loading of a
                      few frequently-used images in your program.
                      
  About the .gd2 image file format
  
                      In addition to reading and writing the PNG format
                      and reading the X Bitmap format, gd has the
                      capability to read and write its own ".gd2" format.
                      This format is _not_ intended for general purpose
                      use and should never be used to distribute images.
                      It is a compressed format allowing pseudo-random
                      access to large image files. Its purpose is solely
                      to allow very fast loading of _parts_ of images If
                      you are experiencing performance problems when
                      loading large, fixed PNG images your program needs
                      to produce its output images, you may wish to
                      examine the functions [354]gdImageCreateFromGd2,
                      [355]gdImageCreateFromGd2Part and [356]gdImageGd2,
                      which read and write .gd2 format images.
                      
                      The program "pngtogd2.c" is provided as a simple
                      way of converting .png files to .gd2 format.
                      
  About the gdIOCtx structure
  
                      Version 1.5 of GD added a new style of I/O based on
                      an IOCtx structure (the most up-to-date version can
                      be found in gd_io.h):
                      

typedef struct gdIOCtx {
        int     (*getC)(struct gdIOCtx*);
        int     (*getBuf)(struct gdIOCtx*, void*, int);

        void     (*putC)(struct gdIOCtx*, int);
        int     (*putBuf)(struct gdIOCtx*, const void*, int);

        int     (*seek)(struct gdIOCtx*, const int);
        long    (*tell)(struct gdIOCtx*);

        void    (*free)(struct gdIOCtx*);

} gdIOCtx;

              Most functions that accepted files in previous versions now
                      also have a counterpart that accepts an I/O
                      context. These functions have a 'Ctx' suffix.
                      
                      The Ctx routines use the function pointers in the
                      I/O context pointed to by gdIOCtx to perform all
                      I/O. Examples of how to implement an I/O context
                      can be found in io_file.c (which provides a wrapper
                      for file routines), and io_dp.c (which implements
                      in-memory storage).
                      
                      It is not necessary to implement all functions in
                      an I/O context if you know that it will only be
                      used in limited cirsumstances. At the time of
                      writing (Version 1.6, June 1999), the known
                      requirements are:
                      
                      All   Must have 'free',
                      Anything that reads from the context Must have
                      'getC' and 'getBuf',
                      Anything that writes to the context Must have
                      'putC' and 'putBuf'.
                      If gdCreateFromGd2Part is called Must also have
                      'seek' and 'tell'.
                      If gdImageGd2 is called Must also have 'seek' and
                      'tell'.
                      
  Please tell us you're using gd!
  
                      When you contact us and let us know you are using
                      gd, you help us justify the time spent in
                      maintaining and improving it. So please let us
                      know. If the results are publicly visible on the
                      web, a URL is a wonderful thing to receive, but if
                      it's not a publicly visible project, a simple note
                      is just as welcome.
                      
  If you have problems
  
                      If you have any difficulties with gd, feel free to
                      contact the author, [357]Thomas Boutell. Problems
                      relating to the gd2 format should be addressed to
                      [358]Philip Warner.
                      
                      _Be sure to read this manual carefully first. _
                      
  Alphabetical quick index
  
                      [359]gdBrushed | [360]gdDashSize | [361]gdFont |
                      [362]gdFontPtr | [363]gdImage | [364]gdImageArc |
                      [365]gdImageBlue | [366]gdImageBoundsSafe |
                      [367]gdImageChar | [368]gdImageCharUp |
                      [369]gdImageColorAllocate |
                      [370]gdImageColorClosest |
                      [371]gdImageColorDeallocate |
                      [372]gdImageColorExact |
                      [373]gdImageColorTransparent | [374]gdImageCopy |
                      [375]gdImageCopyResized | [376]gdImageCreate |
                      [377]gdImageCreateFromGd |
                      [378]gdImageCreateFromGd2 |
                      [379]gdImageCreateFromGd2Part |
                      [380]gdImageCreateFromPng |
                      [381]gdImageCreateFromPngSource |
                      [382]gdImageCreateFromXbm | [383]gdImageDashedLine
                      | [384]gdImageDestroy | [385]gdImageFill |
                      [386]gdImageFillToBorder |
                      [387]gdImageFilledRectangle | [388]gdImageGd |
                      [389]gdImageGd2 | [390]gdImageGetInterlaced |
                      [391]gdImageGetPixel | [392]gdImageGetTransparent |
                      [393]gdImageGreen | [394]gdImageInterlace |
                      [395]gdImageLine | [396]gdImageFilledPolygon |
                      [397]gdImagePaletteCopy | [398]gdImagePng |
                      [399]gdImagePngToSink | [400]gdImagePolygon |
                      [401]gdImagePtr | [402]gdImageRectangle |
                      [403]gdImageRed | [404]gdImageSetBrush |
                      [405]gdImageSetPixel | [406]gdImageSetStyle |
                      [407]gdImageSetTile | [408]gdImageString |
                      [409]gdImageString16 | [410]gdImageStringUp |
                      [411]gdImageStringUp16 | [412]gdMaxColors |
                      [413]gdPoint | [414]gdStyled | [415]gdStyledBrushed
                      | [416]gdTiled | [417]gdTransparent
                      
                      _[418]Boutell.Com, Inc._

References

   1. http://www.boutell.com/gd/
   2. file://localhost/mnt/hdb/boutell/gd1.5/index.html#notice
   3. file://localhost/mnt/hdb/boutell/gd1.5/index.html#whatsnew1.6
   4. file://localhost/mnt/hdb/boutell/gd1.5/index.html#whatis
   5. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdother
   6. file://localhost/mnt/hdb/boutell/gd1.5/index.html#required
   7. file://localhost/mnt/hdb/boutell/gd1.5/index.html#getgd
   8. file://localhost/mnt/hdb/boutell/gd1.5/index.html#buildgd
   9. file://localhost/mnt/hdb/boutell/gd1.5/index.html#basics
  10. file://localhost/mnt/hdb/boutell/gd1.5/index.html#webgif
  11. file://localhost/mnt/hdb/boutell/gd1.5/index.html#reference
  12. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdformat
  13. file://localhost/mnt/hdb/boutell/gd1.5/index.html#informing
  14. file://localhost/mnt/hdb/boutell/gd1.5/index.html#problems
  15. file://localhost/mnt/hdb/boutell/gd1.5/index.html#index
  16. http://www.boutell.com/
  17. mailto:boutell@boutell.com
  18. http://www-genome.wi.mit.edu/ftp/pub/software/WWW/GD.html
  19. http://s27w007.pswfs.gov/tgd/
  20. http://www.unimelb.edu.au/fly/fly.html
  21. mailto:pjw@rhyme.com.au
  22. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPngSource
  23. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePngToSink
  24. http://www.cdrom.com/pub/infozip/zlib/
  25. http://www.cdrom.com/pub/png/
  26. http://www.boutell.com/gd/http/gd1.6.tar.gz
  27. http://www.boutell.com/gd/http/gd16.zip
  28. ftp://ftp.boutell.com/pub/boutell/gd/gd1.6.tar.gz
  29. ftp://ftp.boutell.com/pub/boutell/gd/gd16.zip
  30. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  31. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
  32. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
  33. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
  34. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageLine
  35. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePng
  36. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
  37. file://localhost/mnt/hdb/boutell/gd1.5/index.html#index
  38. file://localhost/mnt/hdb/boutell/gd1.5/index.html#types
  39. file://localhost/mnt/hdb/boutell/gd1.5/index.html#creating
  40. file://localhost/mnt/hdb/boutell/gd1.5/index.html#drawing
  41. file://localhost/mnt/hdb/boutell/gd1.5/index.html#query
  42. file://localhost/mnt/hdb/boutell/gd1.5/index.html#fonts
  43. file://localhost/mnt/hdb/boutell/gd1.5/index.html#colors
  44. file://localhost/mnt/hdb/boutell/gd1.5/index.html#copying
  45. file://localhost/mnt/hdb/boutell/gd1.5/index.html#misc
  46. file://localhost/mnt/hdb/boutell/gd1.5/index.html#constants
  47. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
  48. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
  49. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  50. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePolygon
  51. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFilledPolygon
  52. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdPoint
  53. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePolygon
  54. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFilledPolygon
  55. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPngSource
  56. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePngToSink
  57. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  58. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
  59. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  60. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
  61. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdioctx
  62. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  63. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
  64. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  65. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
  66. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPng
  67. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdSource
  68. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPng
  69. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdioctx
  70. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdformat
  71. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  72. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
  73. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  74. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
  75. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdioctx
  76. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdformat
  77. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  78. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
  79. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  80. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
  81. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdioctx
  82. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdformat
  83. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromGd2
  84. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  85. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
  86. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  87. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
  88. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  89. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
  90. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  91. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  92. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
  93. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
  94. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
  95. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
  96. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
  97. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
  98. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageRectangle
  99. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 100. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePng
 101. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdSink
 102. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePng
 103. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdformat
 104. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 105. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 106. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 107. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 108. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageRectangle
 109. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 110. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gd2format
 111. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 112. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 113. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 114. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 115. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageRectangle
 116. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 117. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImage
 118. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 119. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 120. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 121. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 122. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 123. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 124. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdStyled
 125. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdBrushed
 126. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdStyledBrushed
 127. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 128. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 129. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 130. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 131. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 132. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageLine
 133. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageSetStyle
 134. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 135. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 136. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 137. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 138. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 139. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFilledPolygon
 140. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 141. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdPoint
 142. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 143. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 144. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 145. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 146. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 147. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 148. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 149. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 150. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 151. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFilledPolygon
 152. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 153. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdPoint
 154. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 155. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 156. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 157. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 158. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePolygon
 159. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 160. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 161. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 162. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 163. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 164. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 165. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 166. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 167. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 168. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 169. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 170. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFill
 171. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdTiled
 172. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 173. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 174. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 175. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 176. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 177. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 178. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFillToBorder
 179. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdTiled
 180. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageTransparent
 181. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 182. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 183. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 184. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 185. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 186. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 187. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorTransparent
 188. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageLine
 189. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePolygon
 190. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdBrushed
 191. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdStyledBrushed
 192. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 193. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdBrushed
 194. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 195. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 196. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPng
 197. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 198. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageLine
 199. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdBrushed
 200. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 201. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 202. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorTransparent
 203. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFill
 204. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFilledPolygon
 205. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdTiled
 206. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 207. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdBrushed
 208. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 209. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 210. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPng
 211. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 212. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFilledRectangle
 213. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdTiled
 214. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 215. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 216. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdTransparent
 217. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageLine
 218. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdStyled
 219. file://localhost/mnt/hdb/boutell/gd1.5/index.html#whatsnew1.1.1
 220. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdStyledBrushed
 221. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 222. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 223. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 224. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 225. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageLine
 226. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdStyled
 227. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageLine
 228. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdStyled
 229. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 230. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImage
 231. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPng
 232. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 233. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 234. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 235. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 236. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageString
 237. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageString16
 238. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCharUp
 239. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 240. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 241. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 242. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 243. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 244. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageStringUp
 245. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageStringUp16
 246. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageChar
 247. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 248. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 249. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 250. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 251. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 252. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageChar
 253. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageStringUp
 254. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 255. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 256. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 257. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 258. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 259. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageChar
 260. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageStringUp16
 261. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageString
 262. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCharUp
 263. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageString
 264. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 265. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 266. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 267. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 268. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 269. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageChar
 270. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageStringUp16
 271. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageStringUp
 272. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdMaxColors
 273. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorExact
 274. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorClosest
 275. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 276. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 277. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 278. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 279. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 280. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdMaxColors
 281. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorExact
 282. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 283. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPng
 284. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 285. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 286. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorClosest
 287. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 288. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPng
 289. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 290. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 291. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 292. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 293. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 294. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPng
 295. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 296. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 297. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 298. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPng
 299. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorExact
 300. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePng
 301. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 302. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCopyResized
 303. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorExact
 304. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 305. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorClosest
 306. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 307. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 308. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPng
 309. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 310. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePng
 311. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 312. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 313. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCopy
 314. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorExact
 315. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 316. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorClosest
 317. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 318. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 319. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPng
 320. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 321. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePng
 322. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 323. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 324. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCopy
 325. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCopyMerge
 326. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPng
 327. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePng
 328. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 329. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageLine
 330. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageRectangle
 331. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageSetBrush
 332. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdStyledBrushed
 333. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageLine
 334. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageRectangle
 335. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageSetStyle
 336. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdTransparent
 337. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorTransparent
 338. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageLine
 339. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageRectangle
 340. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageSetBrush
 341. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageSetStyle
 342. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdStyled
 343. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFilledRectangle
 344. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFilledPolygon
 345. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFill
 346. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFillToBorder
 347. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageSetTile
 348. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFill
 349. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFillToBorder
 350. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageSetStyle
 351. file://localhost/mnt/hdb/boutell/gd1.5/gdImageColorTransparent
 352. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromGd
 353. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageGd
 354. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromGd2
 355. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromGd2Part
 356. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageGd2
 357. http://www.boutell.com/boutell/
 358. mailto:pjw@thyme.com.au
 359. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdBrushed
 360. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdDashSize
 361. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdFont
 362. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdFontPtr
 363. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImage
 364. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageArc
 365. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageBlue
 366. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageBoundsSafe
 367. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageChar
 368. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCharUp
 369. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorAllocate
 370. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorClosest
 371. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorDeallocate
 372. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorExact
 373. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageColorTransparent
 374. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCopy
 375. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCopyResized
 376. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreate
 377. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromGd
 378. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromGd2
 379. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromGd2Part
 380. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPng
 381. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromPngSource
 382. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageCreateFromXbm
 383. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDashedLine
 384. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageDestroy
 385. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFill
 386. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFillToBorder
 387. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFilledRectangle
 388. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageGd
 389. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageGd
 390. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageGetInterlaced
 391. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageGetPixel
 392. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageGetTransparent
 393. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageGreen
 394. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageInterlace
 395. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageLine
 396. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageFilledPolygon
 397. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePaletteCopy
 398. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePng
 399. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePngToSink
 400. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePolygon
 401. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImagePtr
 402. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageRectangle
 403. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageRed
 404. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageSetBrush
 405. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageSetPixel
 406. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageSetStyle
 407. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageSetTile
 408. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageString
 409. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageString16
 410. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageStringUp
 411. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdImageStringUp
 412. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdMaxColors
 413. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdPoint
 414. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdStyled
 415. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdStyledBrushed
 416. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdTiled
 417. file://localhost/mnt/hdb/boutell/gd1.5/index.html#gdTransparent
 418. http://www.boutell.com/
