SRC_DIR =C:\Users\nathanael\Documents\gd-libgd
CC="cl.exe "
LINK=C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\BIN\link.exe
LD="$(LINK)"
MT=C:\Program Files (x86)\Windows Kits\8.1\bin\x86\mt.exe
MT="$(MT)"
NMAKE=nmake.exe 
MAKE_LIB=lib.exe 

BASE_INCLUDES=/I . /I src /I deps/include

CFLAGS_PHP=/D _USRDLL /D WINVER=0x500 
CFLAGS_PHP_OBJ=$(CFLAGS_PHP) $(STATIC_EXT_CFLAGS) 

BUILD_DIR=Release_GD
BUILD_DIRS_SUB=src ext\pcre ext\pcre\pcrelib ext\reflection

LIBS=kernel32.lib ole32.lib user32.lib advapi32.lib shell32.lib ws2_32.lib Dnsapi.lib 
LIBS_GD=libjpeg_a.lib freetype_a.lib libpng_a.lib libiconv_a.lib libxpm_a.lib User32.Lib Gdi32.Lib zlib_a.lib

CFLAGS_GD_OBJ=$(CFLAGS_PHP) $(CFLAGS_GD) 
CFLAGS_BD_EXT_GD_LIBGD= /Fd$(BUILD_DIR)\src\ /Fp$(BUILD_DIR)\src\ /FR$(BUILD_DIR)\src\  
CFLAGS_GD=/DHAVE_FT2BUILD_H=1  /DHAVE_GD_H=1 /DHAVE_PNG_H=1 /DHAVE_ICONV_H=1 /DHAVE_XPM_H=1 -Isrc -Ideps/include -Ideps/include/libpng15 -Ideps/include/vpx -Ideps/include/freetype /D HAVE_GD_DYNAMIC_CTX_EX=1 /D HAVE_GD_BUNDLED=1  /D HAVE_GD_GD2  /D HAVE_GD_GIF_READ=1  /D HAVE_GD_GIF_CREATE=1  /D HAVE_GDIMAGECOLORRESOLVE=1  /D HAVE_GD_IMAGESETBRUSH=1  /D HAVE_GD_IMAGESETTILE=1 /D HAVE_GD_FONTCACHESHUTDOWN=1 /D HAVE_GD_FONTMUTEX=1 /D HAVE_LIBFREETYPE=1 /D HAVE_GD_JPG  /D HAVE_GD_PNG  /D HAVE_GD_STRINGFTEX=1  /D HAVE_GD_STRINGTTF=1  /D HAVE_GD_WBMP  /D HAVE_GD_XBM  /D HAVE_GD_XPM  /D HAVE_GD_FREETYPE=1  /D HAVE_LIBGD13=1  /D HAVE_LIBGD15=1  /D HAVE_LIBGD20=1  /D HAVE_LIBGD204=1 /D HAVE_LIBJPEG  /D HAVE_LIBPNG  /D HAVE_XPM  /D HAVE_COLORCLOSESTHWB  /D USE_GD_IMGSTRTTF  /D USE_GD_IOCTX /D MSWIN32 		 


GD_GLOBAL_OBJS=  $(BUILD_DIR)\src\gd.obj $(BUILD_DIR)\src\gd2copypal.obj $(BUILD_DIR)\src\gd_arc.obj $(BUILD_DIR)\src\gd_arc_f_buggy.obj $(BUILD_DIR)\src\gd_color.obj $(BUILD_DIR)\src\gd_crop.obj $(BUILD_DIR)\src\gd_filter.obj $(BUILD_DIR)\src\gd_gd.obj $(BUILD_DIR)\src\gd_gd2.obj $(BUILD_DIR)\src\gd_gif_in.obj $(BUILD_DIR)\src\gd_gif_out.obj $(BUILD_DIR)\src\gd_interpolation.obj $(BUILD_DIR)\src\gd_io.obj $(BUILD_DIR)\src\gd_io_dp.obj $(BUILD_DIR)\src\gd_io_file.obj $(BUILD_DIR)\src\gd_io_ss.obj $(BUILD_DIR)\src\gd_jpeg.obj $(BUILD_DIR)\src\gd_matrix.obj $(BUILD_DIR)\src\gd_pixelate.obj $(BUILD_DIR)\src\gd_png.obj $(BUILD_DIR)\src\gd_rotate.obj $(BUILD_DIR)\src\gd_security.obj $(BUILD_DIR)\src\gd_ss.obj $(BUILD_DIR)\src\gd_topal.obj $(BUILD_DIR)\src\gd_transform.obj $(BUILD_DIR)\src\gd_wbmp.obj $(BUILD_DIR)\src\gd_webp.obj $(BUILD_DIR)\src\gdcache.obj $(BUILD_DIR)\src\gdfontg.obj $(BUILD_DIR)\src\gdfontl.obj $(BUILD_DIR)\src\gdfontmb.obj $(BUILD_DIR)\src\gdfonts.obj $(BUILD_DIR)\src\gdfontt.obj $(BUILD_DIR)\src\gdft.obj $(BUILD_DIR)\src\gdhelpers.obj $(BUILD_DIR)\src\gdkanji.obj $(BUILD_DIR)\src\gdtables.obj $(BUILD_DIR)\src\gdxpm.obj $(BUILD_DIR)\src\wbmp.obj $(BUILD_DIR)\src\webpimg.obj $(BUILD_DIR)\src\xbm.obj 

GD_DLL=libgd.dll

DLL_LDFLAGS=/dll

PHP_LDFLAGS=$(DLL_LDFLAGS) /nodefaultlib:libcmt 

LDFLAGS=/nologo /libpath:"$(SRC_DIR)\deps\lib"  



_VC_MANIFEST_EMBED_EXE= if exist $@.manifest $(MT) -nologo -manifest $@.manifest -outputresource:$@;1
_VC_MANIFEST_EMBED_DLL= if exist $@.manifest $(MT) -nologo -manifest $@.manifest -outputresource:$@;2



STATIC_EXT_LDFLAGS=$(LDFLAGS_DATE) $(LDFLAGS_GD) $(LDFLAGS_PCRE) $(LDFLAGS_REFLECTION) $(LDFLAGS_SPL) $(LDFLAGS_STANDARD) $(LDFLAGS_ZLIB) 





all: build_dirs $(BUILD_DIR)\$(GD_DLL)

build_dirs: $(BUILD_DIR) $(BUILD_DIRS_SUB)

$(BUILD_DIR) $(BUILD_DIRS_SUB):
	@echo Recreating build dirs
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@cd $(BUILD_DIR)
	@for %D in ($(BUILD_DIRS_SUB)) do @if not exist %D @mkdir %D > NUL
	@cd "$(SRC_DIR)"



$(BUILD_DIR)\$(GD_DLL): $(GD_GLOBAL_OBJS) 
	@$(CC)  $(GD_GLOBAL_OBJS) $(LIBS_GD)  $(LIBS) /link /out:$(BUILD_DIR)\$(PHPDLL)  $(PHP_LDFLAGS) $(LDFLAGS) $(LDFLAGS_GD) 
	-@$(_VC_MANIFEST_EMBED_DLL)

$(BUILD_DIR)\ext\gd\gd.obj: ext\gd\gd.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD) /c ext\gd\gd.c /Fo$(BUILD_DIR)\ext\gd\gd.obj

$(BUILD_DIR)\src\gd.obj: src\gd.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd.c /Fo$(BUILD_DIR)\src\gd.obj
$(BUILD_DIR)\src\gd2copypal.obj: src\gd2copypal.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd2copypal.c /Fo$(BUILD_DIR)\src\gd2copypal.obj
$(BUILD_DIR)\src\gd_arc.obj: src\gd_arc.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_arc.c /Fo$(BUILD_DIR)\src\gd_arc.obj
$(BUILD_DIR)\src\gd_arc_f_buggy.obj: src\gd_arc_f_buggy.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_arc_f_buggy.c /Fo$(BUILD_DIR)\src\gd_arc_f_buggy.obj
$(BUILD_DIR)\src\gd_color.obj: src\gd_color.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_color.c /Fo$(BUILD_DIR)\src\gd_color.obj
$(BUILD_DIR)\src\gd_crop.obj: src\gd_crop.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_crop.c /Fo$(BUILD_DIR)\src\gd_crop.obj
$(BUILD_DIR)\src\gd_filter.obj: src\gd_filter.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_filter.c /Fo$(BUILD_DIR)\src\gd_filter.obj
$(BUILD_DIR)\src\gd_gd.obj: src\gd_gd.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_gd.c /Fo$(BUILD_DIR)\src\gd_gd.obj
$(BUILD_DIR)\src\gd_gd2.obj: src\gd_gd2.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_gd2.c /Fo$(BUILD_DIR)\src\gd_gd2.obj
$(BUILD_DIR)\src\gd_gif_in.obj: src\gd_gif_in.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_gif_in.c /Fo$(BUILD_DIR)\src\gd_gif_in.obj
$(BUILD_DIR)\src\gd_gif_out.obj: src\gd_gif_out.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_gif_out.c /Fo$(BUILD_DIR)\src\gd_gif_out.obj
$(BUILD_DIR)\src\gd_interpolation.obj: src\gd_interpolation.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_interpolation.c /Fo$(BUILD_DIR)\src\gd_interpolation.obj
$(BUILD_DIR)\src\gd_io.obj: src\gd_io.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_io.c /Fo$(BUILD_DIR)\src\gd_io.obj
$(BUILD_DIR)\src\gd_io_dp.obj: src\gd_io_dp.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_io_dp.c /Fo$(BUILD_DIR)\src\gd_io_dp.obj
$(BUILD_DIR)\src\gd_io_file.obj: src\gd_io_file.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_io_file.c /Fo$(BUILD_DIR)\src\gd_io_file.obj
$(BUILD_DIR)\src\gd_io_ss.obj: src\gd_io_ss.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_io_ss.c /Fo$(BUILD_DIR)\src\gd_io_ss.obj
$(BUILD_DIR)\src\gd_jpeg.obj: src\gd_jpeg.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_jpeg.c /Fo$(BUILD_DIR)\src\gd_jpeg.obj
$(BUILD_DIR)\src\gd_matrix.obj: src\gd_matrix.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_matrix.c /Fo$(BUILD_DIR)\src\gd_matrix.obj
$(BUILD_DIR)\src\gd_pixelate.obj: src\gd_pixelate.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_pixelate.c /Fo$(BUILD_DIR)\src\gd_pixelate.obj
$(BUILD_DIR)\src\gd_png.obj: src\gd_png.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_png.c /Fo$(BUILD_DIR)\src\gd_png.obj
$(BUILD_DIR)\src\gd_rotate.obj: src\gd_rotate.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_rotate.c /Fo$(BUILD_DIR)\src\gd_rotate.obj
$(BUILD_DIR)\src\gd_security.obj: src\gd_security.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_security.c /Fo$(BUILD_DIR)\src\gd_security.obj
$(BUILD_DIR)\src\gd_ss.obj: src\gd_ss.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_ss.c /Fo$(BUILD_DIR)\src\gd_ss.obj
$(BUILD_DIR)\src\gd_topal.obj: src\gd_topal.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_topal.c /Fo$(BUILD_DIR)\src\gd_topal.obj
$(BUILD_DIR)\src\gd_transform.obj: src\gd_transform.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_transform.c /Fo$(BUILD_DIR)\src\gd_transform.obj
$(BUILD_DIR)\src\gd_wbmp.obj: src\gd_wbmp.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_wbmp.c /Fo$(BUILD_DIR)\src\gd_wbmp.obj
$(BUILD_DIR)\src\gd_webp.obj: src\gd_webp.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gd_webp.c /Fo$(BUILD_DIR)\src\gd_webp.obj
$(BUILD_DIR)\src\gdcache.obj: src\gdcache.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gdcache.c /Fo$(BUILD_DIR)\src\gdcache.obj
$(BUILD_DIR)\src\gdfontg.obj: src\gdfontg.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gdfontg.c /Fo$(BUILD_DIR)\src\gdfontg.obj
$(BUILD_DIR)\src\gdfontl.obj: src\gdfontl.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gdfontl.c /Fo$(BUILD_DIR)\src\gdfontl.obj
$(BUILD_DIR)\src\gdfontmb.obj: src\gdfontmb.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gdfontmb.c /Fo$(BUILD_DIR)\src\gdfontmb.obj
$(BUILD_DIR)\src\gdfonts.obj: src\gdfonts.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gdfonts.c /Fo$(BUILD_DIR)\src\gdfonts.obj
$(BUILD_DIR)\src\gdfontt.obj: src\gdfontt.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gdfontt.c /Fo$(BUILD_DIR)\src\gdfontt.obj
$(BUILD_DIR)\src\gdft.obj: src\gdft.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gdft.c /Fo$(BUILD_DIR)\src\gdft.obj
$(BUILD_DIR)\src\gdhelpers.obj: src\gdhelpers.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gdhelpers.c /Fo$(BUILD_DIR)\src\gdhelpers.obj
$(BUILD_DIR)\src\gdkanji.obj: src\gdkanji.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gdkanji.c /Fo$(BUILD_DIR)\src\gdkanji.obj
$(BUILD_DIR)\src\gdtables.obj: src\gdtables.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gdtables.c /Fo$(BUILD_DIR)\src\gdtables.obj
$(BUILD_DIR)\src\gdxpm.obj: src\gdxpm.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\gdxpm.c /Fo$(BUILD_DIR)\src\gdxpm.obj
$(BUILD_DIR)\src\wbmp.obj: src\wbmp.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\wbmp.c /Fo$(BUILD_DIR)\src\wbmp.obj
$(BUILD_DIR)\src\webpimg.obj: src\webpimg.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\webpimg.c /Fo$(BUILD_DIR)\src\webpimg.obj
$(BUILD_DIR)\src\xbm.obj: src\xbm.c
	@$(CC) $(CFLAGS_GD_OBJ) $(CFLAGS) $(CFLAGS_BD_EXT_GD_LIBGD) /c src\xbm.c /Fo$(BUILD_DIR)\src\xbm.obj


