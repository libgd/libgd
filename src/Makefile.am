## Process this file with automake to produce Makefile.in -*-Makefile-*-

bin_PROGRAMS = gdcmpgif
check_PROGRAMS = gifanimtest gd_color_map_test

if HAVE_LIBPNG
bin_PROGRAMS += gdtopng pngtogd webpng
check_PROGRAMS += circletexttest fontsizetest fontwheeltest gddemo testac
if HAVE_LIBZ
bin_PROGRAMS += gdparttopng gd2topng pngtogd2
check_PROGRAMS += gdtest
endif
endif

if HAVE_LIBJPEG
if !HAVE_LIBPNG
check_PROGRAMS += fontsizetest fontwheeltest
endif
endif

if HAVE_LIBFONTCONFIG
check_PROGRAMS += fontconfigtest
endif

if HAVE_LIBFREETYPE
if HAVE_LIBJPEG
bin_PROGRAMS += annotate
endif
check_PROGRAMS += gdtestft testtr
endif

if HAVE_LIBZ
bin_PROGRAMS += gd2copypal gd2togif giftogd2
endif

bin_SCRIPTS = bdftogd

if HAVE_LIBZ
check_PROGRAMS += gd2time
endif

EXTRA_DIST = \
	bdftogd \
	demoin.png \
	entities.html \
	entities.tcl \
	CMakeLists.txt \
	config.h.cmake \
	gd_io_stream.cxx \
	gdpp.cxx \
	getopt.c \
	snprintf.c \
	msinttypes/inttypes.h \
	msinttypes/stdint.h

include_HEADERS = gd.h gdfx.h gd_io.h gdcache.h gdfontg.h gdfontl.h gdfontmb.h gdfonts.h gdfontt.h entities.h gd_color_map.h gd_errors.h gdpp.h

lib_LTLIBRARIES = libgd.la

libgd_la_SOURCES = \
	bmp.h \
	gd.c \
	gd.h \
	gd_bmp.c \
	gd_color.c \
	gd_color.h \
	gd_color_map.c \
	gd_color_map.h \
	gd_color_match.c \
	gd_crop.c \
	gd_filename.c \
	gd_filter.c \
	gd_gd.c \
	gd_gd2.c \
	gd_gif_in.c \
	gd_gif_out.c \
	gd_intern.h \
	gd_interpolation.c \
	gd_io.c \
	gd_io.h \
	gd_io_dp.c \
	gd_io_file.c \
	gd_io_ss.c \
	gd_io_stream.h \
	gd_jpeg.c \
	gd_matrix.c \
	gd_nnquant.c \
	gd_nnquant.h \
	gd_png.c \
	gd_rotate.c \
	gd_security.c \
	gd_ss.c \
	gd_tga.c \
	gd_tga.h \
	gd_tiff.c \
	gd_topal.c \
	gd_transform.c \
	gd_version.c \
	gd_wbmp.c \
	gd_webp.c \
	gd_xbm.c \
	gdcache.c \
	gdcache.h \
	gdfontg.c \
	gdfontg.h \
	gdfontl.c \
	gdfontl.h \
	gdfontmb.c \
	gdfontmb.h \
	gdfonts.c \
	gdfonts.h \
	gdfontt.c \
	gdfontt.h \
	gdft.c \
	gdfx.c \
	gdfx.h \
	gdhelpers.c \
	gdhelpers.h \
	gdkanji.c \
	gdtables.c \
	gdxpm.c \
	jisx0208.h \
	wbmp.c \
	wbmp.h

libgd_la_LDFLAGS = -version-info $(GDLIB_LT_CURRENT):$(GDLIB_LT_REVISION):$(GDLIB_LT_AGE) -no-undefined

libgd_la_LIBADD = $(LTLIBICONV)

LDADD = libgd.la $(LIBICONV)
