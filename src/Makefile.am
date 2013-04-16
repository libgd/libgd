## Process this file with automake to produce Makefile.in -*-Makefile-*-
bin_PROGRAMS = annotate gdparttopng gdtopng gd2copypal gd2topng pngtogd pngtogd2 webpng gd2togif gdcmpgif giftogd2

bin_SCRIPTS = bdftogd

check_PROGRAMS = fontsizetest fontwheeltest gdtest gddemo gd2time gdtestft testac circletexttest testtr fontconfigtest gifanimtest gd_color_map_test

EXTRA_DIST = bdftogd demoin.png entities.html entities.tcl CMakeLists.txt

include_HEADERS = gd.h gdfx.h gd_io.h gdcache.h gdfontg.h gdfontl.h gdfontmb.h gdfonts.h gdfontt.h entities.h gd_color_map.h

lib_LTLIBRARIES = libgd.la

libgd_la_SOURCES = gd.c gd_color.c gd_color_map.c gd_transform.c gdfx.c gd_security.c gd_gd.c gd_gd2.c gd_io.c gd_io_dp.c gd_gif_in.c gd_gif_out.c gd_io_file.c gd_io_ss.c gd_jpeg.c gd_png.c gd_ss.c gd_topal.c gd_wbmp.c gdcache.c gdfontg.c gdfontl.c gdfontmb.c gdfonts.c gdfontt.c gdft.c gdhelpers.c gdhelpers.h gdkanji.c gdtables.c gdxpm.c jisx0208.h wbmp.c wbmp.h gd_filter.c gd_nnquant.c gd_rotate.c gd_matrix.c gd_interpolation.c gd_crop.c webpimg.c gd_webp.c gd_tiff.c

libgd_la_LDFLAGS = -version-info $(GDLIB_CURRENT):$(GDLIB_REVISION):$(GDLIB_AGE) $(XTRA_LDFLAGS)

libgd_la_LIBADD = $(LTLIBICONV)

LDADD = libgd.la $(LIBICONV)

