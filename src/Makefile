#Depending on your system, you will need to modify this makefile.

#If you do not have gcc, change the setting for COMPILER, but you must
#use an ANSI standard C compiler (NOT the old SunOS 4.1.3 cc
#compiler; get gcc if you are still using it). 
COMPILER=gcc

#If the ar command fails on your system, consult the ar manpage
#for your system. 
AR=ar

#If you don't have FreeType, libjpeg and/or Xpm installed, including the
#header files, uncomment this (default). You really must install
#libpng and zlib to get anywhere if you wish to create PNG images.
CFLAGS=-O -DHAVE_LIBPNG -DHAVE_LIBJPEG

#If you do have FreeType, libjpeg and/or Xpm fully installed, uncomment a
#variation of this and comment out the line above. See also LIBS below.
#CFLAGS=-O -DHAVE_LIBXPM -DHAVE_LIBPNG -DHAVE_LIBJPEG \
#	-DHAVE_LIBFREETYPE -DHAVE_LIBTTF 

#To use the old FreeType 1.x library, add this additional #define
#to the line above
#-DHAVE_LIBTTF 

#If you don't have FreeType Xpm fully installed, uncomment this
#(default).

#PLEASE NOTE: YOU MAY HAVE TO JUGGLE THE ORDER OF THE LIBRARIES.
#Some systems are very picky about link order. They don't all agree
#on the right order, either.

LIBS=-lgd -lpng -lz -lm

#If you do have FreeType, JPEG and/or Xpm fully installed, uncomment a 
#variation of this and comment out the line above. Note that
#Xpm requires X11. See also CFLAGS above.

#LIBS=-lgd -lpng -lz -ljpeg -lfreetype -lm -lttf

#Note: for Freetype 1.x, use DHAVE_LIBTTF and -lttf instead.

#Typical install locations for freetype, zlib, xpm, libjpeg and libpng header 
#files. If yours are somewhere else, change this. -I. is important to 
#ensure that the version of gd you are installing is used, and not an 
#older release in your directory tree somewhere.

INCLUDEDIRS=-I. -I/usr/include/freetype2 -I/usr/include/X11 -I/usr/X11R6/include/X11 -I/usr/local/include 

#Typical install locations for freetype, zlib, xpm and libpng libraries.
#If yours are somewhere else, other than a standard location
#such as /lib or /usr/lib, then change this. Be sure to keep
#-L. as this allows the gd library itself to be found.
#Put -L. first so that old versions of the gd library elsewhere
#on your system can't cause conflicts while building a new one.
#This line shouldn't hurt if you don't actually have some of the
#optional libraries and directories.
LIBDIRS=-L. -L/usr/local/lib -L/usr/lib/X11 -L/usr/X11R6/lib

#Location where libgd.a should be installed by "make install".
INSTALL_LIB=/usr/local/lib

#Location where .h files should be installed by "make install".
INSTALL_INCLUDE=/usr/local/include

#Location where useful non-test programs should be installed by "make install".
INSTALL_BIN=/usr/local/bin

#
#
# Changes should not be required below here.
#
#

VERSION=1.8.4

CC=$(COMPILER) $(INCLUDEDIRS)
LINK=$(CC) $(LIBDIRS) $(LIBS)

PROGRAMS=$(BIN_PROGRAMS) $(TEST_PROGRAMS)

BIN_PROGRAMS=pngtogd pngtogd2 gdtopng gd2topng gd2copypal gdparttopng webpng
TEST_PROGRAMS=gdtest gddemo gd2time gdtestft gdtestttf

all: libgd.a $(PROGRAMS)

install: libgd.a $(BIN_PROGRAMS)
	sh ./install-item 644 libgd.a $(INSTALL_LIB)/libgd.a
	sh ./install-item 755 pngtogd $(INSTALL_BIN)/pngtogd
	sh ./install-item 755 pngtogd2 $(INSTALL_BIN)/pngtogd2
	sh ./install-item 755 gdtopng $(INSTALL_BIN)/gdtopng
	sh ./install-item 755 gd2topng $(INSTALL_BIN)/gd2topng
	sh ./install-item 755 gd2copypal $(INSTALL_BIN)/gd2copypal
	sh ./install-item 755 gdparttopng $(INSTALL_BIN)/gdparttopng
	sh ./install-item 755 webpng $(INSTALL_BIN)/webpng
	sh ./install-item 755 bdftogd $(INSTALL_BIN)/bdftogd
	sh ./install-item 644 gd.h $(INSTALL_INCLUDE)/gd.h
	sh ./install-item 644 gdcache.h $(INSTALL_INCLUDE)/gdcache.h
	sh ./install-item 644 gd_io.h $(INSTALL_INCLUDE)/gd_io.h
	sh ./install-item 644 gdfontg.h $(INSTALL_INCLUDE)/gdfontg.h
	sh ./install-item 644 gdfontl.h $(INSTALL_INCLUDE)/gdfontl.h
	sh ./install-item 644 gdfontmb.h $(INSTALL_INCLUDE)/gdfontmb.h
	sh ./install-item 644 gdfonts.h $(INSTALL_INCLUDE)/gdfonts.h
	sh ./install-item 644 gdfontt.h $(INSTALL_INCLUDE)/gdfontt.h

gddemo: gddemo.o libgd.a
	$(CC) gddemo.o -o gddemo	$(LIBDIRS) $(LIBS)

pngtogd: pngtogd.o libgd.a
	$(CC) pngtogd.o -o pngtogd	$(LIBDIRS) $(LIBS) 

webpng: webpng.o libgd.a
	$(CC) webpng.o -o webpng	$(LIBDIRS) $(LIBS)

pngtogd2: pngtogd2.o libgd.a
	$(CC) pngtogd2.o -o pngtogd2	$(LIBDIRS) $(LIBS)

gdtopng: gdtopng.o libgd.a
	$(CC) gdtopng.o -o gdtopng	$(LIBDIRS) $(LIBS)

gd2topng: gd2topng.o libgd.a
	$(CC) gd2topng.o -o gd2topng	$(LIBDIRS) $(LIBS)

gd2copypal: gd2copypal.o libgd.a
	$(CC) gd2copypal.o -o gd2copypal	$(LIBDIRS) $(LIBS)

gdparttopng: gdparttopng.o libgd.a
	$(CC) gdparttopng.o -o gdparttopng	$(LIBDIRS) $(LIBS)

gdtest: gdtest.o libgd.a
	$(CC) gdtest.o -o gdtest	$(LIBDIRS) $(LIBS)

gd2time: gd2time.o libgd.a
	$(CC) gd2time.o -o gd2time	$(LIBDIRS) $(LIBS)

gdtestft: gdtestft.o libgd.a
	$(CC) --verbose gdtestft.o -o gdtestft $(LIBDIRS) $(LIBS)
gdtestttf: gdtestttf.o libgd.a
	$(CC) --verbose gdtestttf.o -o gdtestttf $(LIBDIRS) $(LIBS)

libgd.a: gd.o gd_gd.o gd_gd2.o gd_io.o gd_io_dp.o gd_io_file.o gd_ss.o \
	gd_io_ss.o gd_png.o gd_jpeg.o gdxpm.o gdfontt.o gdfonts.o gdfontmb.o gdfontl.o \
	gdfontg.o gdtables.o gdft.o gdttf.o gdcache.o gdkanji.o wbmp.o \
	gd_wbmp.o gdhelpers.o gd.h gdfontt.h gdfonts.h gdfontmb.h gdfontl.h \
	gdfontg.h gdhelpers.h
	rm -f libgd.a
	$(AR) rc libgd.a gd.o gd_gd.o gd_gd2.o gd_io.o gd_io_dp.o \
		gd_io_file.o gd_ss.o gd_io_ss.o gd_png.o gd_jpeg.o gdxpm.o \
		gdfontt.o gdfonts.o gdfontmb.o gdfontl.o gdfontg.o \
		gdtables.o gdft.o gdttf.o gdcache.o gdkanji.o wbmp.o \
		gd_wbmp.o gdhelpers.o
	-ranlib libgd.a

clean:
	rm -f *.o *.a ${PROGRAMS} test/gdtest.jpg test/gdtest.wbmp

