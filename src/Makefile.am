## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.4

noinst_HEADERS = gdcache.h 

lib_LTLIBRARIES = libgd.la

include_HEADERS = gd.h gd_io.h gdfontt.h gdfonts.h \
	gdfontmb.h gdfontl.h gdfontg.h

libgd_la_SOURCES = gd.c gd_io.c gd_io_file.c gd_io_dp.c gd_io_ss.c \
	gd_png.c gd_ss.c gd_gd.c gd_gd2.c gdfontt.c gdfonts.c gdfontmb.c \
	gdfontl.c gdfontg.c gdttf.c gdcache.c \
	gd.h gdcache.h gdfontl.h gdfonts.h gd_io.h gdfontg.h \
	gdfontmb.h gdfontt.h

libgd_la_LIBADD = @LIBS@ -lm

bin_PROGRAMS = pngtogd pngtogd2 gdtopng gd2topng gd2copypal gdparttopng webpng

noinst_PROGRAMS = gdtest gddemo gd2time gdtestttf

gdtest_SOURCES = gdtest.c gd.h
gdtest_LDADD = libgd.la

gdtestttf_SOURCES = gdtestttf.c gd.h
gdtestttf_LDADD = libgd.la

gddemo_SOURCES = gddemo.c gd.h gdfonts.h gdfontg.h
gddemo_LDADD = libgd.la

pngtogd_SOURCES = pngtogd.c gd.h
pngtogd_LDADD = libgd.la

pngtogd2_SOURCES = pngtogd2.c gd.h
pngtogd2_LDADD = libgd.la

gdtopng_SOURCES = gdtopng.c gd.h
gdtopng_LDADD = libgd.la

gd2topng_SOURCES = gd2topng.c gd.h
gd2topng_LDADD = libgd.la

gd2copypal_SOURCES = gd2copypal.c gd.h
gd2copypal_LDADD = libgd.la

gdparttopng_SOURCES = gdparttopng.c gd.h
gdparttopng_LDADD = libgd.la

gd2time_SOURCES = gd2time.c gd.h
gd2time_LDADD = libgd.la

webpng_SOURCES = webpng.c gd.h
webpng_LDADD = libgd.la

EXTRA_DIST = demoin.png index.html test/*

CLEANFILES = demoout.png
