
# Simple makefile for building and testing libgd under MSYS/MinGW on
# Windows


# The version
LIBVER=3.0.1

# Misc. config flags.
CDEFS=-DHAVE_ERRNO_H -DHAVE_ICONV -DHAVE_ICONV_H		\
-DHAVE_ICONV_T_DEF -DHAVE_INTTYPES_H -DHAVE_LIMITS_H -DHAVE_STDDEF_H	\
-DHAVE_STDINT_H -DHAVE_STDLIB_H -DHAVE_VISIBILITY -DICONV_CONST

# GnuWin32 libs are all installed in one place
GNUINC=-I/c/Progra~1/GnuWin32/include
GNULIB=-L/c/Progra~1/GnuWin32/lib

# Various optional components.  Comment-out the ones you don't have
# and edit the paths and options as needed if you do.  The default
# assumes GnuWin32 packages installed in /c/Program Files/GnuWin32.
JPEG_D=-DHAVE_LIBJPEG
JPEG_L=-ljpeg
JPEG_I=$(GNUINC)
JPEG_LIBDIR=$(GNULIB)

PNG_D=-DHAVE_LIBPNG
PNG_L=-lpng
PNG_I=				#$(GNUINC)
PNG_LIBDIR=			#$(GNULIB)

TIFF_D=-DHAVE_LIBTIFF
TIFF_L=-ltiff
TIFF_I=				#$(GNUINC)
TIFF_LIBDIR=		#$(GNULIB)

LIBZ_D=-DHAVE_LIBZ
LIBZ_L=-lz
LIBZ_I=				#$(GNUINC)
LIBZ_LIBDIR=		#$(GNULIB)

# LIBXPM_D=-DHAVE_LIBXPM
# LIBXPM_L=-lxpm
# LIBXPM_I=			#$(GNUINC)
# LIBXPM_LIBDIR=		#$(GNULIB)

FREETYPE_D=-DHAVE_LIBFREETYPE -DHAVE_FT2BUILD_H
FREETYPE_L=-lfreetype
FREETYPE_I=$(GNUINC)/freetype2/
FREETYPE_LIBDIR=	#$(GNULIB)

# FONTCONFIG_D=-DHAVE_LIBFONTCONFIG
# FONTCONFIG_L=
# FONTCONFIG_I=
# FONTCONFIG_LIBDIR=

ALL_D=$(FREETYPE_D) $(JPEG_D) $(PNG_D) $(TIFF_D) $(LIBZ_D) $(LIBXPM_D)
ALL_L=$(FREETYPE_L) $(JPEG_L) $(PNG_L) $(TIFF_L) $(LIBZ_L) $(LIBXPM_L)
ALL_I=$(FREETYPE_I) $(JPEG_I) $(PNG_I) $(TIFF_I) $(LIBZ_I) $(LIBXPM_I)
ALL_LIBDIR=$(FREETYPE_LIBDIR) $(JPEG_LIBDIR) $(PNG_LIBDIR) $(TIFF_LIBDIR) \
	$(LIBZ_LIBDIR) $(LIBXPM_LIBDIR)


DEFS=$(CDEFS) $(ALL_D)
INCLUDES=$(ALL_I)

CC=gcc
CFLAGS=-g -O2 -I. -std=gnu99 -fvisibility=hidden -D_WIN32 -DBGDWIN32 $(DEFS) $(INCLUDES)

LD=gcc
LDFLAGS=-g -O2 -fvisibility=hidden $(ALL_LIBDIR)
LIBS=-liconv $(ALL_L)

# Get this list from c files in libgd_la_SOURCES in Makefile.mk
SRC=gd.c gd_color.c gd_color_map.c gd_transform.c gdfx.c			\
gd_security.c gd_gd.c gd_gd2.c gd_io.c gd_io_dp.c gd_gif_in.c		\
gd_gif_out.c gd_io_file.c gd_io_ss.c gd_jpeg.c gd_png.c gd_ss.c		\
gd_topal.c gd_wbmp.c gdcache.c gdfontg.c gdfontl.c gdfontmb.c		\
gdfonts.c gdfontt.c gdft.c gdhelpers.c gdkanji.c gdtables.c gdxpm.c	\
wbmp.c gd_filter.c gd_nnquant.c gd_rotate.c gd_matrix.c				\
gd_interpolation.c gd_crop.c webpimg.c gd_webp.c gd_tiff.c gd_tga.c	\
gd_bmp.c gd_xbm.c gd_color_match.c gd_version.c

OBJ=$(SRC:.c=.o)

TARGETBASE=libgd.$(LIBVER)
TARGET=$(TARGETBASE).dll
TARGET_A=$(TARGETBASE).a

all:
	(cd ../../src; make -f ../windows/msys/Makefile $(TARGET))

clean:
	(cd ../../src; rm -f $(TARGET) $(OBJ) $(TARGET_A) deps.mk)

check: all
	bash run_tests.sh "$(INCLUDES)"

deps.mk:
	[ -f gd.h ] # Sanity check: we're in src/, right?
	gcc -MM $(DEFS) $(INCLUDES) $(SRC) > deps.mk

$(TARGET): $(OBJ)
	gcc -shared -o $(TARGET) $(LDFLAGS) -Wl,--out-implib,$(TARGET_A) $(OBJ) $(LIBS)

include deps.mk










