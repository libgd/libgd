SRC_DIR =C:\Users\nathanael\Documents\gd-libgd
CC="cl.exe "
LINK=C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\BIN\link.exe
LD="$(LINK)"
MT=C:\Program Files (x86)\Windows Kits\8.1\bin\x86\mt.exe
MT="$(MT)"
NMAKE=nmake.exe 
MAKE_LIB=lib.exe 

BASE_INCLUDES=/I . /I src /I deps/include

CFLAGS_PHP=/D _USRDLL /D WINVER=0x500 /TC /D _WIN32=1 /D BGDWIN32=1
CFLAGS_PHP_OBJ=$(CFLAGS_PHP) $(STATIC_EXT_CFLAGS) 

BUILD_DIR=Release_GD
BUILD_DIRS_SUB=src ext\pcre ext\pcre\pcrelib ext\reflection

LIBS=kernel32.lib ole32.lib user32.lib advapi32.lib shell32.lib ws2_32.lib Dnsapi.lib 
LIBS_GD=libjpeg_a.lib freetype_a.lib libpng_a.lib libiconv_a.lib libxpm_a.lib User32.Lib Gdi32.Lib zlib_a.lib

CFLAGS_GD_OBJ=$(CFLAGS_PHP) $(CFLAGS_GD) /TC
CFLAGS_BD_EXT_GD_LIBGD= /Fd$(SDR) /Fp$(SDR) /FR$(SDR)  
CFLAGS_GD=/DHAVE_FT2BUILD_H=1  /DHAVE_GD_H=1 /DHAVE_PNG_H=1 /DHAVE_ICONV_H=1 /DHAVE_XPM_H=1 -Isrc -Ideps/include -Ideps/include/libpng15 -Ideps/include/vpx -Ideps/include/freetype /D HAVE_GD_DYNAMIC_CTX_EX=1 /D HAVE_GD_BUNDLED=1  /D HAVE_GD_GD2  /D HAVE_GD_GIF_READ=1  /D HAVE_GD_GIF_CREATE=1  /D HAVE_GDIMAGECOLORRESOLVE=1  /D HAVE_GD_IMAGESETBRUSH=1  /D HAVE_GD_IMAGESETTILE=1 /D HAVE_GD_FONTCACHESHUTDOWN=1 /D HAVE_GD_FONTMUTEX=1 /D HAVE_LIBFREETYPE=1 /D HAVE_GD_JPG  /D HAVE_GD_PNG  /D HAVE_GD_STRINGFTEX=1  /D HAVE_GD_STRINGTTF=1  /D HAVE_GD_WBMP  /D HAVE_GD_XBM  /D HAVE_GD_XPM  /D HAVE_GD_FREETYPE=1  /D HAVE_LIBGD13=1  /D HAVE_LIBGD15=1  /D HAVE_LIBGD20=1  /D HAVE_LIBGD204=1 /D HAVE_LIBJPEG  /D HAVE_LIBPNG  /D HAVE_XPM  /D HAVE_COLORCLOSESTHWB  /D USE_GD_IMGSTRTTF  /D USE_GD_IOCTX /D MSWIN32 		 


SDR = $(BUILD_DIR)
GD_GLOBAL_OBJS=$(SDR)gd.obj $(SDR)gd_color.obj $(SDR)gd_color_map.obj $(SDR)gd_transform.obj $(SDR)gdfx.obj $(SDR)gd_security.obj $(SDR)gd_gd.obj $(SDR)gd_gd2.obj $(SDR)gd_io.obj $(SDR)gd_io_dp.obj $(SDR)gd_gif_in.obj $(SDR)gd_gif_out.obj $(SDR)gd_io_file.obj $(SDR)gd_io_ss.obj $(SDR)gd_jpeg.obj $(SDR)gd_png.obj $(SDR)gd_ss.obj $(SDR)gd_topal.obj $(SDR)gd_wbmp.obj $(SDR)gdcache.obj $(SDR)gdfontg.obj $(SDR)gdfontl.obj $(SDR)gdfontmb.obj $(SDR)gdfonts.obj $(SDR)gdfontt.obj $(SDR)gdft.obj $(SDR)gdhelpers.obj $(SDR)gdkanji.obj $(SDR)gdtables.obj $(SDR)gdxpm.obj $(SDR)wbmp.obj $(SDR)gd_filter.obj $(SDR)gd_nnquant.obj $(SDR)gd_rotate.obj $(SDR)gd_matrix.obj $(SDR)gd_interpolation.obj $(SDR)gd_crop.obj $(SDR)webpimg.obj $(SDR)gd_webp.obj $(SDR)gd_tiff.obj $(SDR)gd_tga.obj $(SDR)gd_bmp.obj $(SDR)gd_xbm.obj $(SDR)gd_color_match.obj $(SDR)gd_version.obj $(SDR)gd_filename.obj



TARGETBASE=libgd
TARGET=$(TARGETBASE).dll
TARGET_A=$(TARGETBASE).a


GD_DLL=libgd.dll

DLL_LDFLAGS=/DLL /D _USRDLL /D _WINDLL

PHP_LDFLAGS=$(DLL_LDFLAGS) /nodefaultlib:libcmt 

LDFLAGS=/nologo /libpath:"$(SRC_DIR)\deps\lib" 



_VC_MANIFEST_EMBED_EXE= if exist $@.manifest $(MT) -nologo -manifest $@.manifest -outputresource:$@;1
_VC_MANIFEST_EMBED_DLL= if exist $@.manifest $(MT) -nologo -manifest $@.manifest -outputresource:$@;2



STATIC_EXT_LDFLAGS=$(LDFLAGS_DATE) $(LDFLAGS_GD) $(LDFLAGS_PCRE) $(LDFLAGS_REFLECTION) $(LDFLAGS_SPL) $(LDFLAGS_STANDARD) $(LDFLAGS_ZLIB) 


CCWF=$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) 


all: build_dirs $(BUILD_DIR)\$(GD_DLL)

build_dirs: $(BUILD_DIR) $(BUILD_DIRS_SUB)

$(BUILD_DIR) $(BUILD_DIRS_SUB):
	@echo Recreating build dirs
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@cd $(BUILD_DIR)
	@for %D in ($(BUILD_DIRS_SUB)) do @if not exist %D @mkdir %D > NUL
	@cd "$(SRC_DIR)"


$(BUILD_DIR)\$(GD_DLL): $(GD_GLOBAL_OBJS) 
	@$(CC) /D_USRDLL /D_WINDLL $(GD_GLOBAL_OBJS) $(LIBS_GD)  $(LIBS) /link /out:$(BUILD_DIR)\$(GD_DLL)  $(PHP_LDFLAGS) $(LDFLAGS) $(LDFLAGS_GD) 
	-@$(_VC_MANIFEST_EMBED_DLL)



	


$(SDR)gd.obj: src\gd.c
	@$(CCWF) /c src\gd.c /Fo$(SDR)gd.obj 
$(SDR)gd_color.obj: src\gd_color.c
	@$(CCWF) /c src\gd_color.c /Fo$(SDR)gd_color.obj 
$(SDR)gd_color_map.obj: src\gd_color_map.c
	@$(CCWF) /c src\gd_color_map.c /Fo$(SDR)gd_color_map.obj 
$(SDR)gd_transform.obj: src\gd_transform.c
	@$(CCWF) /c src\gd_transform.c /Fo$(SDR)gd_transform.obj 
$(SDR)gdfx.obj: src\gdfx.c
	@$(CCWF) /c src\gdfx.c /Fo$(SDR)gdfx.obj			

$(SDR)gd_security.obj: src\gd_security.c
	@$(CCWF) /c src\gd_security.c /Fo$(SDR)gd_security.obj 
$(SDR)gd_gd.obj: src\gd_gd.c
	@$(CCWF) /c src\gd_gd.c /Fo$(SDR)gd_gd.obj 
$(SDR)gd_gd2.obj: src\gd_gd2.c
	@$(CCWF) /c src\gd_gd2.c /Fo$(SDR)gd_gd2.obj 
$(SDR)gd_io.obj: src\gd_io.c
	@$(CCWF) /c src\gd_io.c /Fo$(SDR)gd_io.obj 
$(SDR)gd_io_dp.obj: src\gd_io_dp.c
	@$(CCWF) /c src\gd_io_dp.c /Fo$(SDR)gd_io_dp.obj 
$(SDR)gd_gif_in.obj: src\gd_gif_in.c
	@$(CCWF) /c src\gd_gif_in.c /Fo$(SDR)gd_gif_in.obj		

$(SDR)gd_gif_out.obj: src\gd_gif_out.c
	@$(CCWF) /c src\gd_gif_out.c /Fo$(SDR)gd_gif_out.obj 
$(SDR)gd_io_file.obj: src\gd_io_file.c
	@$(CCWF) /c src\gd_io_file.c /Fo$(SDR)gd_io_file.obj 
$(SDR)gd_io_ss.obj: src\gd_io_ss.c
	@$(CCWF) /c src\gd_io_ss.c /Fo$(SDR)gd_io_ss.obj 
$(SDR)gd_jpeg.obj: src\gd_jpeg.c
	@$(CCWF) /c src\gd_jpeg.c /Fo$(SDR)gd_jpeg.obj 
$(SDR)gd_png.obj: src\gd_png.c
	@$(CCWF) /c src\gd_png.c /Fo$(SDR)gd_png.obj 
$(SDR)gd_ss.obj: src\gd_ss.c
	@$(CCWF) /c src\gd_ss.c /Fo$(SDR)gd_ss.obj		

$(SDR)gd_topal.obj: src\gd_topal.c
	@$(CCWF) /c src\gd_topal.c /Fo$(SDR)gd_topal.obj 
$(SDR)gd_wbmp.obj: src\gd_wbmp.c
	@$(CCWF) /c src\gd_wbmp.c /Fo$(SDR)gd_wbmp.obj 
$(SDR)gdcache.obj: src\gdcache.c
	@$(CCWF) /c src\gdcache.c /Fo$(SDR)gdcache.obj 
$(SDR)gdfontg.obj: src\gdfontg.c
	@$(CCWF) /c src\gdfontg.c /Fo$(SDR)gdfontg.obj 
$(SDR)gdfontl.obj: src\gdfontl.c
	@$(CCWF) /c src\gdfontl.c /Fo$(SDR)gdfontl.obj 
$(SDR)gdfontmb.obj: src\gdfontmb.c
	@$(CCWF) /c src\gdfontmb.c /Fo$(SDR)gdfontmb.obj		

$(SDR)gdfonts.obj: src\gdfonts.c
	@$(CCWF) /c src\gdfonts.c /Fo$(SDR)gdfonts.obj 
$(SDR)gdfontt.obj: src\gdfontt.c
	@$(CCWF) /c src\gdfontt.c /Fo$(SDR)gdfontt.obj 
$(SDR)gdft.obj: src\gdft.c
	@$(CCWF) /c src\gdft.c /Fo$(SDR)gdft.obj 
$(SDR)gdhelpers.obj: src\gdhelpers.c
	@$(CCWF) /c src\gdhelpers.c /Fo$(SDR)gdhelpers.obj 
$(SDR)gdkanji.obj: src\gdkanji.c
	@$(CCWF) /c src\gdkanji.c /Fo$(SDR)gdkanji.obj 
$(SDR)gdtables.obj: src\gdtables.c
	@$(CCWF) /c src\gdtables.c /Fo$(SDR)gdtables.obj 
$(SDR)gdxpm.obj: src\gdxpm.c
	@$(CCWF) /c src\gdxpm.c /Fo$(SDR)gdxpm.obj	

$(SDR)wbmp.obj: src\wbmp.c
	@$(CCWF) /c src\wbmp.c /Fo$(SDR)wbmp.obj 
$(SDR)gd_filter.obj: src\gd_filter.c
	@$(CCWF) /c src\gd_filter.c /Fo$(SDR)gd_filter.obj 
$(SDR)gd_nnquant.obj: src\gd_nnquant.c
	@$(CCWF) /c src\gd_nnquant.c /Fo$(SDR)gd_nnquant.obj 
$(SDR)gd_rotate.obj: src\gd_rotate.c
	@$(CCWF) /c src\gd_rotate.c /Fo$(SDR)gd_rotate.obj 
$(SDR)gd_matrix.obj: src\gd_matrix.c
	@$(CCWF) /c src\gd_matrix.c /Fo$(SDR)gd_matrix.obj				

$(SDR)gd_interpolation.obj: src\gd_interpolation.c
	@$(CCWF) /c src\gd_interpolation.c /Fo$(SDR)gd_interpolation.obj 
$(SDR)gd_crop.obj: src\gd_crop.c
	@$(CCWF) /c src\gd_crop.c /Fo$(SDR)gd_crop.obj 
$(SDR)webpimg.obj: src\webpimg.c
	@$(CCWF) /c src\webpimg.c /Fo$(SDR)webpimg.obj 
$(SDR)gd_webp.obj: src\gd_webp.c
	@$(CCWF) /c src\gd_webp.c /Fo$(SDR)gd_webp.obj 
$(SDR)gd_tiff.obj: src\gd_tiff.c
	@$(CCWF) /c src\gd_tiff.c /Fo$(SDR)gd_tiff.obj 
$(SDR)gd_tga.obj: src\gd_tga.c
	@$(CCWF) /c src\gd_tga.c /Fo$(SDR)gd_tga.obj	

$(SDR)gd_bmp.obj: src\gd_bmp.c
	@$(CCWF) /c src\gd_bmp.c /Fo$(SDR)gd_bmp.obj 
$(SDR)gd_xbm.obj: src\gd_xbm.c
	@$(CCWF) /c src\gd_xbm.c /Fo$(SDR)gd_xbm.obj 
$(SDR)gd_color_match.obj: src\gd_color_match.c
	@$(CCWF) /c src\gd_color_match.c /Fo$(SDR)gd_color_match.obj 
$(SDR)gd_version.obj: src\gd_version.c
	@$(CCWF) /c src\gd_version.c /Fo$(SDR)gd_version.obj 
$(SDR)gd_filename.obj: src\gd_filename.c
	@$(CCWF) /c src\gd_filename.c /Fo$(SDR)gd_filename.obj

#([^\s]+).c
#\n$(SDR)\1.obj: src\\\1.c\n\t@$(CCWF) /c src\\\1.c /Fo$(SDR)\1.obj