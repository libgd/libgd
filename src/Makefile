#Depending on your system, you will need to modify this makefile.

#If you do not have gcc, change the setting for CC, but you must
#use an ANSI standard C compiler (NOT the old SunOS 4.1.3 cc
#compiler; get gcc if you are still using it). 

#If the ar command fails on your system, consult the ar manpage
#for your system. 

CC=gcc 
AR=ar
#CFLAGS= -ggdb
CFLAGS= -g 
LIBS=-L. -lgd -lz -lm -lpng

all: libgd.a gdtest gddemo pngtogd pngtogd2 gdtopng gd2topng gd2copypal gdparttopng gd2time webpng

gdtest:	gdtest.o libgd.a gd.h
	$(CC) gdtest.o -o gdtest        $(LIBS)

gddemo: gddemo.o libgd.a gd.h gdfonts.h gdfontl.h
	$(CC) gddemo.o -o gddemo	$(LIBS)

pngtogd: pngtogd.o libgd.a gd.h
	$(CC) pngtogd.o -o pngtogd	$(LIBS) 

pngtogd2: pngtogd2.o libgd.a gd.h
	$(CC) pngtogd2.o -o pngtogd2      $(LIBS)

gdtopng: gdtopng.o libgd.a gd.h
	$(CC) gdtopng.o -o gdtopng      $(LIBS)

gd2topng: gd2topng.o libgd.a gd.h
	$(CC) gd2topng.o -o gd2topng      $(LIBS)

gd2copypal: gd2copypal.o libgd.a gd.h
	$(CC) gd2copypal.o -o gd2copypal      $(LIBS)

gdparttopng: gdparttopng.o libgd.a gd.h
	$(CC) gdparttopng.o -o gdparttopng      $(LIBS)

gd2time: gd2time.o libgd.a gd.h
	$(CC) gd2time.o -o gd2time      $(LIBS)

libgd.a: gd.o gd_io.o gd_io_file.o gd_io_dp.o gd_io_ss.o gd_png.o gd_ss.o \
	gd_gd.o gd_gd2.o gdfontt.o gdfonts.o gdfontmb.o gdfontl.o gdfontg.o \
	gd.h gdfontt.h gdfonts.h gdfontmb.h gdfontl.h gdfontg.h
	rm -f libgd.a
	$(AR) rc libgd.a gd.o gd_io.o gd_io_file.o gd_io_dp.o gd_io_ss.o \
		gd_png.o gd_ss.o gd_gd.o gd_gd2.o \
		gdfontt.o gdfonts.o gdfontmb.o gdfontl.o gdfontg.o

webpng: webpng.o libgd.a gd.h
	$(CC) webpng.o -o webpng	$(LIBS)

clean:
	rm -f *.o *.a gddemo gdtest pngtogd pngtogd2 gdtopng gd2topng gdparttopng gd2time gd2copypal webpng 

