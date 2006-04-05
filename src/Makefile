#Depending on your system, you will need to modify this makefile.

#If you do not have gcc, change the setting for COMPILER, but you must
#use an ANSI standard C compiler (NOT the old SunOS 4.1.3 cc
#compiler; get gcc if you are still using it). 
COMPILER=gcc

#If the ar command fails on your system, consult the ar manpage
#for your system. 
AR=ar

#If the install command is not in your path, provide
#an explicit path for it here, or install manually.
INSTALL=install

#If you don't have FreeType and/or Xpm installed, including the
#header files, uncomment this (default).
CFLAGS=-O
#If you do have FreeType and/or Xpm fully installed, uncomment a
#variation of this and comment out the line above. See also LIBS below.
#CFLAGS=-O -DHAVE_XPM -DHAVE_LIBTTF

#If you don't have FreeType and/or Xpm fully installed, uncomment this
#(default).
LIBS=-lm -lz -lpng -lgd 
#If you do have FreeType and/or Xpm fully installed, uncomment a 
#variation of this and comment out the line above. Note that
#Xpm requires X11. See also CFLAGS above.
#LIBS=-lm -lttf -lz -lpng -lXpm -lX11 -lgd 

#Typical install locations for freetype, zlib, xpm and libpng header files.
#If yours are somewhere else, change this. 
INCLUDEDIRS=-I/usr/local/include -I/usr/include/X11 -I/usr/X11R6/include/X11

#Typical install locations for freetype, zlib, xpm and libpng libraries.
#If yours are somewhere else, other than a standard location
#such as /lib or /usr/lib, then change this. Be sure to keep
#-L. as this allows the gd library itself to be found.
#Put -L. first so that old versions of the gd library elsewhere
#on your system can't cause conflicts while building a new one.
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

VERSION=1.7

CC=$(COMPILER) $(INCLUDEDIRS)
LINK=$(CC) $(LIBDIRS) $(LIBS)

PROGRAMS=$(BIN_PROGRAMS) $(TEST_PROGRAMS)

BIN_PROGRAMS=pngtogd pngtogd2 gdtopng gd2topng gd2copypal gdparttopng webpng
TEST_PROGRAMS=gdtest gddemo gd2time gdtestttf

all: libgd.a $(PROGRAMS)

install: libgd.a $(BIN_PROGRAMS)
	$(INSTALL) -m 644 libgd.a $(INSTALL_LIB)/libgd.a
	$(INSTALL) -m 755 pngtogd $(INSTALL_BIN)/pngtogd
	$(INSTALL) -m 755 pngtogd2 $(INSTALL_BIN)/pngtogd2
	$(INSTALL) -m 755 gdtopng $(INSTALL_BIN)/gdtopng
	$(INSTALL) -m 755 gd2topng $(INSTALL_BIN)/gd2topng
	$(INSTALL) -m 755 gd2copypal $(INSTALL_BIN)/gd2copypal
	$(INSTALL) -m 755 gdparttopng $(INSTALL_BIN)/gdparttopng
	$(INSTALL) -m 755 webpng $(INSTALL_BIN)/webpng
	$(INSTALL) -m 755 bdftogd $(INSTALL_BIN)/bdftogd
	$(INSTALL) -m 644 gd.h $(INSTALL_INCLUDE)/gd.h
	$(INSTALL) -m 644 gdcache.h $(INSTALL_INCLUDE)/gdcache.h
	$(INSTALL) -m 644 gd_io.h $(INSTALL_INCLUDE)/gd_io.h
	$(INSTALL) -m 644 gdfontg.h $(INSTALL_INCLUDE)/gdfontg.h
	$(INSTALL) -m 644 gdfontl.h $(INSTALL_INCLUDE)/gdfontl.h
	$(INSTALL) -m 644 gdfontmb.h $(INSTALL_INCLUDE)/gdfontmb.h
	$(INSTALL) -m 644 gdfonts.h $(INSTALL_INCLUDE)/gdfonts.h
	$(INSTALL) -m 644 gdfontt.h $(INSTALL_INCLUDE)/gdfontt.h

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

gdtestttf: gdtestttf.o libgd.a
	$(CC) gdtestttf.o -o gdtestttf	$(LIBDIRS) $(LIBS)

libgd.a: gd.o gd_gd.o gd_gd2.o gd_io.o gd_io_dp.o gd_io_file.o gd_ss.o \
	gd_io_ss.o gd_png.o gdxpm.o gdfontt.o gdfonts.o gdfontmb.o gdfontl.o \
	gdfontg.o gdtables.o gdttf.o gdcache.o gdkanji.o \
	gd.h gdfontt.h gdfonts.h gdfontmb.h gdfontl.h gdfontg.h
	rm -f libgd.a
	$(AR) rc libgd.a gd.o gd_gd.o gd_gd2.o gd_io.o gd_io_dp.o \
		gd_io_file.o gd_ss.o gd_io_ss.o gd_png.o gdxpm.o \
		gdfontt.o gdfonts.o gdfontmb.o gdfontl.o gdfontg.o \
		gdtables.o gdttf.o gdcache.o gdkanji.o
	-ranlib libgd.a

clean:
	rm -f *.o *.a ${PROGRAMS}

