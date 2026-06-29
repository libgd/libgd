## Process this file with automake to produce Makefile.in -*-Makefile-*-

bin_PROGRAMS = gdcmpgif
check_PROGRAMS = gifanimtest

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
	msinttypes/inttypes.h \
	msinttypes/stdint.h

include_HEADERS = gd.h gd_vector2d.h gdfx.h gd_io.h gdcache.h gdfontg.h gdfontl.h gdfontmb.h gdfonts.h gdfontt.h gd_color_map.h gd_errors.h gdpp.h

lib_LTLIBRARIES = libgd.la

libgd_la_SOURCES = \
	bmp.h \
	entities.h \
	gd.c \
	gd.h \
	gd_avif.c \
	gd_bmp.c \
	gd_color.c \
	gd_color.h \
	gd_color_map.c \
	gd_color_map.h \
	gd_color_match.c \
	gd_compositor.c gd_compositor.h \
	gd_crop.c \
	gd_draw.c gd_draw_blend.c gd_draw_blend.h \
	gd_filename.c \
	gd_filter.c \
	gd_gradient.c gd_gradient.h \
	gd_gd.c \
	gd_gd2.c \
	gd_gif_in.c \
	gd_gif_out.c \
	gd_heif.c \
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
	gd_metadata.c \
	gd_nnquant.c \
	gd_nnquant.h \
	gd_path.c gd_path.h gd_path_arc.c gd_path_arc.h gd_path_dash.c gd_path_dash.h \
	gd_path_matrix.c gd_path_matrix.h gd_path_outline.h gd_path_stroke.c \
	gd_png.c \
	gd_qoi.c \
	gd_qoi.h \
	gd_rotate.c \
	gd_security.c \
	gd_ss.c \
	gd_span_rle.c gd_span_rle.h gd_surface.c gd_surface.h \
	gd_tga.c \
	gd_tga.h \
	gd_tiff.c \
	gd_topal.c \
	gd_transform.c \
	gd_uhdr.c \
	gd_version.c \
	gd_wbmp.c \
	gd_webp.c \
	gd_xbm.c \
	gd_array.c gd_array.h gd_fixed.h gd_vector2d.h gd_vector2d_private.h \
	ftraster/gd_ft_math.c ftraster/gd_ft_math.h \
	ftraster/gd_ft_raster.c ftraster/gd_ft_raster.h \
	ftraster/gd_ft_stroker.c ftraster/gd_ft_stroker.h ftraster/gd_ft_types.h \
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
