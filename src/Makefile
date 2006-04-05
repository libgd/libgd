#Depending on your system, you will need to modify this makefile.

#If you do not have gcc, change the setting for CC, but you must
#use an ANSI standard C compiler (NOT the old SunOS 4.1.3 cc
#compiler; get gcc if you are still using it). 

#If the ar command fails on your system, consult the ar manpage
#for your system. 

CC=gcc 
AR=ar
#CFLAGS= -ggdb
CFLAGS= -O 
LIBS=-L. -lgd -lz -lm

all: libgd.a gdtest gddemo giftolzw giftogd giftogd2 gdtogif gd2togif gd2tolzw gd2copypal gdparttogif gd2time webgif

gdtest:	gdtest.o libgd.a gd.h
	$(CC) gdtest.o -o gdtest        $(LIBS)

gddemo: gddemo.o libgd.a gd.h gdfonts.h gdfontl.h
	$(CC) gddemo.o -o gddemo	$(LIBS)

giftogd: giftogd.o libgd.a gd.h
	$(CC) giftogd.o -o giftogd	$(LIBS) 

giftogd2: giftogd2.o libgd.a gd.h
	$(CC) giftogd2.o -o giftogd2      $(LIBS)

gdtogif: gdtogif.o libgd.a gd.h
	$(CC) gdtogif.o -o gdtogif      $(LIBS)

giftolzw: giftolzw.o libgd.a gd.h
	$(CC) giftolzw.o -o giftolzw      $(LIBS)

gd2togif: gd2togif.o libgd.a gd.h
	$(CC) gd2togif.o -o gd2togif      $(LIBS)

gd2tolzw: gd2tolzw.o libgd.a gd.h
	$(CC) gd2tolzw.o -o gd2tolzw      $(LIBS)

gd2copypal: gd2copypal.o libgd.a gd.h
	$(CC) gd2copypal.o -o gd2copypal      $(LIBS)

gdparttogif: gdparttogif.o libgd.a gd.h
	$(CC) gdparttogif.o -o gdparttogif      $(LIBS)

gd2time: gd2time.o libgd.a gd.h
	$(CC) gd2time.o -o gd2time      $(LIBS)

libgd.a: gd.o io.o io_file.o io_dp.o io_ss.o gd_gif_in.o gd_gif_out.o gd_lzw_out.o gd_gd.o gd_gd2.o gdfontt.o gdfonts.o gdfontmb.o gdfontl.o gdfontg.o \
	gd.h gdfontt.h gdfonts.h gdfontmb.h gdfontl.h gdfontg.h
	rm -f libgd.a
	$(AR) rc libgd.a gd.o io.o io_file.o io_dp.o gd_gif_in.o gd_gif_out.o gd_lzw_out.o gd_gd.o gd_gd2.o gdfontt.o gdfonts.o gdfontmb.o \
		gdfontl.o gdfontg.o

webgif: webgif.o libgd.a gd.h
	$(CC) webgif.o -o webgif	$(LIBS)

clean:
	rm -f *.o *.a gddemo gdtest giftogd giftogd2 giftolzw gdtogif gd2togif gd2tolzw gdparttogif gd2time gd2copypal webgif 

