$! (c) Alexey Chupahin  18-APR-2006, 15-DEC-2007
$! OpenVMS 8.2  Alpha DEC 2000 model 300
$! OpenVMS 8.3  Itanium rx1620
$! GNU
$!
$!
$DEF SYS$ERROR _NL0:
$ SET NOON
$WRITE SYS$OUTPUT " "
$WRITE SYS$OUTPUT "Configuring GD library for OpenVMS"
$WRITE SYS$OUTPUT "(c) Alexey Chupahin "
$! Checking architecture
$! Checking architecture
$    IF F$GETSYI("ARCH_TYPE").EQ.1 THEN CPU = "VAX"
$    IF F$GETSYI("ARCH_TYPE").EQ.2 THEN CPU = "Alpha"
$    IF F$GETSYI("ARCH_TYPE").EQ.3 THEN CPU = "I64"
$    OS = F$GETSYI("VERSION")
$WRITE SYS$OUTPUT "Checking architecture   ...  ", CPU
$WRITE SYS$OUTPUT "Checking OS             ...  OpenVMS ",OS
$SHARED=0
$IF ( (CPU.EQS."Alpha").OR.(CPU.EQS."I64") )
$  THEN
$       SHARED=64
$  ELSE
$       SHARED=32
$ENDIF
$DECC = F$SEARCH("SYS$SYSTEM:DECC$COMPILER.EXE") .NES. ""
$IF (DECC) THEN $WRITE SYS$OUTPUT  "Compiler		...  DEC C"
$IF (.NOT. DECC) THEN $WRITE SYS$OUTPUT  "BAD compiler" GOTO EXIT
$MMS = F$SEARCH("SYS$SYSTEM:MMS.EXE") .NES. ""
$MMK = F$TYPE(MMK)
$IF (MMS .OR. MMK.NES."") THEN GOTO TEST_LIBRARIES
$WRITE SYS$OUTPUT "Please install MMS from DECSET or MMK"
$! I cant find any make tool
$GOTO EXIT
$!
$!
$TEST_LIBRARIES:
$!   Setting as MAKE utility one of MMS or MMK. I prefer MMS.
$IF (MMK.NES."") THEN MAKE="MMK/IGN=WAR"
$IF (MMS) THEN MAKE="MMS/IGN=WAR"
$WRITE SYS$OUTPUT "Checking build utility	...  ''MAKE'"
$WRITE SYS$OUTPUT " "
$!
$IF (P1.EQS."STATIC") .OR. (P1.EQS."static")
$ THEN
$       WRITE SYS$OUTPUT "Configuring for static only"
$       SHARED=0
$ENDIF
$!
$! Is it package root directory? If no, go to [-]
$ IF (F$SEARCH("[]VMS.DIR").EQS."") .AND. (F$SEARCH("[]vms.dir").EQS."")
$  THEN
$	SET DEF [-]
$ ENDIF
$!
$!Checking for iconv
$!
$ DEFINE SYS$OUTPUT _NLA0:
$ CC/OBJECT=TEST.OBJ SYS$INPUT
      #include <iconv.h>
   int main()
     {
        iconv_t cd;
	cd = iconv_open ((const char *)0, (const char *)0);
     }
$TMP = $STATUS
$DEAS  SYS$OUTPUT
$IF (TMP .NE. %X10B90001)
$  THEN
$       HAVE_ICONV=0
$ENDIF
$DEFINE SYS$OUTPUT _NLA0:
$LINK/EXE=TEST TEST
$TMP = $STATUS
$!WRITE SYS$OUTPUT TMP
$IF (TMP .NE. %X10000001)
$  THEN
$       HAVE_ICONV=0
$  ELSE
$       HAVE_ICONV=1
$ENDIF
$DEAS  SYS$OUTPUT
$IF (HAVE_ICONV.EQ.1)
$  THEN
$       WRITE SYS$OUTPUT "Checking for system iconv library...   Yes"
$  ELSE
$       WRITE SYS$OUTPUT "Checking for system iconv library...   No"
$ENDIF
$!
$!"Checking for correct zlib library    "
$ ZLIB_STATIC=0
$ DEFINE SYS$OUTPUT _NLA0:
$ CC/OBJECT=TEST.OBJ/INCLUDE=(ZLIB) SYS$INPUT
      #include <stdlib.h>
      #include <stdio.h>
      #include <zlib.h>
   int main()
     {
        printf("checking version zlib:  %s\n",zlibVersion());
     }
$TMP = $STATUS
$DEAS  SYS$OUTPUT
$IF (TMP .NE. %X10B90001)
$  THEN
$       HAVE_ZLIB=0
$       GOTO NEXT1
$ENDIF
$DEFINE SYS$OUTPUT _NLA0:
$!Testing for shared ZLIB
$COPY SYS$INPUT [.SRC]OPT.OPT
SYS$SHARE:LIBZSHR/SHARE
$LINK/EXE=TEST TEST,[.SRC]OPT/OPT
$TMP = $STATUS
$DEL [.SRC]OPT.OPT;*
$!WRITE SYS$OUTPUT TMP
$IF (TMP .NE. %X10000001)
$  THEN
$	LINK/EXE=TEST TEST,ZLIB:LIBZ/LIB
$	TMP = $STATUS
$	IF (TMP .NE. %X10000001)
$	  THEN
$       	HAVE_ZLIB=0
$	  ELSE
$		HAVE_ZLIB=1
$		ZLIB_STATIC=1
$	ENDIF
$  ELSE
$       HAVE_ZLIB=1
$ENDIF
$NEXT1:
$DEAS  SYS$OUTPUT
$IF (HAVE_ZLIB.EQ.1)
$  THEN
$       WRITE SYS$OUTPUT "Checking for correct zlib library ...   Yes"
$  ELSE
$       WRITE SYS$OUTPUT "Checking for correct zlib library ...   No"
$       WRITE SYS$OUTPUT "This is fatal. Please download and install good library from fafner.dyndns.org/~alexey/libsdl/public.html"
$	GOTO EXIT
$ENDIF
$RUN TEST
$!
$!
$FREETYPE_STATIC=0
$!"Checking for correct Freetype2 library"
$ DEFINE SYS$OUTPUT _NLA0:
$ CC/OBJECT=TEST.OBJ/INCLUDE=(FREETYPE2) SYS$INPUT
#include <ft2build.h>
#include <freetype/freetype.h>
int main(int argc, char *argv[])
{

#if FREETYPE_MAJOR == 2 && FREETYPE_MINOR <= 1
        printf("This version is too old. Please get newest\n");
        printf("From fafner.dyndns.org/~alexey/libsdl/required.html\n");
	exit(2);
#endif
        printf("checking version FreeType  : %d.%d.%d\n",
                        FREETYPE_MAJOR,
                        FREETYPE_MINOR,
                        FREETYPE_PATCH);
        return(0);
}
$TMP = $STATUS
$DEAS  SYS$OUTPUT
$IF (TMP .NE. %X10B90001)
$  THEN
$       HAVE_FREETYPE=0
$       GOTO NEXT0
$ENDIF
$DEAS  SYS$OUTPUT
$DEFINE SYS$OUTPUT _NLA0:
$LINK/EXE=TEST TEST,FREETYPE2:FREETYPE/OPT
$TMP = $STATUS
$IF (TMP .NE. %X10000001)
$  THEN
$	LINK/EXE=TEST TEST,FREETYPE2:FREETYPE/LIB,ZLIB:LIBZ/LIB
$	TMP = $STATUS
$	IF (TMP .NE. %X10000001)
$  	  THEN
$		TMP = $STATUS
$       	HAVE_FREETYPE=0
$       	GOTO NEXT0
$	  ELSE
$		FREETYPE_STATIC=1
$	ENDIF
$ENDIF
$!
$RUN TEST
$IF ($STATUS .NE. %X00000001)
$  THEN
$       HAVE_FREETYPE=0
$	GOTO NEXT0
$  ELSE
$        HAVE_FREETYPE=1
$ENDIF
$NEXT0:
$DEAS  SYS$OUTPUT
$IF (HAVE_FREETYPE.EQ.1)
$  THEN
$       WRITE SYS$OUTPUT "Checking for correct FreeType  ...   Yes"
$	RUN TEST
$  ELSE
$       WRITE SYS$OUTPUT "Checking for correct FreeType  ...   No"
$	WRITE SYS$OUTPUT "This is required library "
$	WRITE SYS$OUTPUT "install it first from fafner.dyndns.org/~alexey/libsdl/required.html"
$	GOTO EXIT
$ENDIF
$!
$!
$HAVE_TIFF=0
$!"Checking for libPNG "
$ DEFINE SYS$OUTPUT _NLA0:
$ CC/OBJECT=TEST.OBJ/INCLUDE=(PNG,ZLIB) SYS$INPUT
	#include <stdlib.h>
        #include  <png.h>
        int main()
        {
	 png_uint_32 n,n1,n2,n3;
	 n=png_access_version_number();
	 n3=n%100; n=n/100;
	 n2=n%100; n=n/100;
	 n1=n%100;
	if ( (n1<=1)&&(n2<2) )
		{ printf("checking version  libPNG:  %d.%d.%d\n",n1,n2,n3);
		  printf("This version is too old. Please get newest\n");
		  printf("From fafner.dyndns.org/~alexey/libsdl/public.html\n");
		  exit(2);
		}
	printf("checking version  libPNG:  %d.%d.%d\n",n1,n2,n3);
        }
$!
$TMP = $STATUS
$DEAS  SYS$OUTPUT
$IF (TMP .NE. %X10B90001)
$  THEN
$       HAVE_LIBPNG=0
$       GOTO NEXT2
$ENDIF
$DEFINE SYS$OUTPUT _NLA0:
$LINK/EXE=TEST TEST,PNG:LIBPNG$SHR/OPT,ZLIB:LIBZ/LIB
$TMP = $STATUS
$!WRITE SYS$OUTPUT TMP
$IF (TMP .NE. %X10000001)
$  THEN
$!	Trying for static libpng
$	LINK/EXE=TEST TEST,PNG:LIBPNG/LIB,ZLIB:LIBZ/LIB
$	TMP = $STATUS
$	IF (TMP .NE. %X10000001)
$	 THEN
$		LIBPNG_STATIC=0
$       	HAVE_LIBPNG=0
$       	GOTO NEXT2
$	 ELSE
$		HAVE_LIBPNG=1
$		LIBPNG_STATIC=1
$       ENDIF
$  ELSE
$        HAVE_LIBPNG=1
$      	 LIBPNG_STATIC=0
$ENDIF
$!
$RUN TEST
$IF ($STATUS .NE. %X00000001)
$  THEN
$	HAVE_LIBPNG=0
$  ELSE
$	 HAVE_LIBPNG=1
$ENDIF
$NEXT2:
$DEAS  SYS$OUTPUT
$IF (HAVE_LIBPNG.EQ.1)
$  THEN
$       WRITE SYS$OUTPUT "Checking for correct  libPNG ...   Yes"
$	RUN TEST
$  ELSE
$       WRITE SYS$OUTPUT "Checking for correct  libPNG ...   No"
$       WRITE SYS$OUTPUT "This is fatal. Please download and install good library from fafner.dyndns.org/~alexey/libsdl/public.html"
$	GOTO EXIT
$ENDIF
$!
$!
$!
$! Checking for JPEG ...
$ DEFINE SYS$OUTPUT _NLA0:
$ CC/OBJECT=TEST.OBJ/INCLUDE=(JPEG) SYS$INPUT
      #include <stdlib.h>
      #include <stdio.h>
      #include <jpeglib.h>
      #include <jversion.h>
   int main()
     {
	printf("checking version jpeg:  %s\n",JVERSION);
	jpeg_quality_scaling(0);
        return 0;
     }
$TMP = $STATUS
$DEAS  SYS$OUTPUT
$!WRITE SYS$OUTPUT TMP
$IF (TMP .NE. %X10B90001)
$  THEN
$	HAVE_JPEG=0
$       GOTO  NEXT3
$ENDIF
$DEFINE SYS$OUTPUT _NLA0:
$LINK/EXE=TEST TEST,JPEG:LIBJPEG$SHR/OPT
$TMP = $STATUS
$!WRITE SYS$OUTPUT TMP
$IF (TMP .NE. %X10000001)
$  THEN
$	LINK/EXE=TEST TEST,JPEG:LIBJPEG/OPT
$	TMP = $STATUS
$	IF (TMP .NE. %X10000001)
$	  THEN
$		HAVE_JPEG=0
$		JPEG_STATIC=0
$		GOTO NEXT3
$	  ELSE
$		HAVE_JPEG=1
$		JPEG_STATIC=1
$	ENDIF
$  ELSE
$	HAVE_JPEG=1
$	JPEG_STATIC=0
$ENDIF
$!
$NEXT3:
$DEAS  SYS$OUTPUT
$IF (HAVE_JPEG.EQ.1)
$  THEN
$       WRITE SYS$OUTPUT "Checking for correct jpeg library ...   Yes"
$  ELSE
$       WRITE SYS$OUTPUT "Checking for correct jpeg library ...   No"
$       WRITE SYS$OUTPUT "This is fatal. Please download and install good library from fafner.dyndns.org/~alexey/libsdl/public.html"
$       GOTO EXIT
$ENDIF
$RUN TEST
$!
$!
$!Checking for libTIFF
$ DEFINE SYS$OUTPUT _NLA0:
$ DEL TEST.*;*
$ CC/OBJECT=TEST.OBJ/INCLUDE=(TIFF) SYS$INPUT
      #include <stdlib.h>
      #include <stdio.h>
      #include <tiffio.h>
   int main()
     {
        printf("checking libTIFF version:\t%s\n\n", TIFFGetVersion());
        return 0;
     }
$!
$TMP = $STATUS
$DEAS  SYS$OUTPUT
$IF (TMP .NE. %X10B90001)
$  THEN
$       HAVE_TIFF=0
$	GOTO NEXT4
$ENDIF
$DEFINE SYS$OUTPUT _NLA0:
$LINK TEST, TIFF:LIBTIFF/OPT, ZLIB:LIBZ/LIB
$TMP = $STATUS
$!WRITE SYS$OUTPUT TMP
$IF (TMP .NE. %X10000001)
$  THEN
$       HAVE_TIFF=0
$  ELSE
$       HAVE_TIFF=1
$ENDIF
$NEXT4:
$DEAS  SYS$OUTPUT
$IF (HAVE_TIFF.EQ.1)
$  THEN
$       WRITE SYS$OUTPUT "Checking for TIFF library ...   Yes"
$       TIFF_LIBRARY_PATH="TIFF:LIBTIFF/OPT"
$       RUN TEST
$  ELSE
$       WRITE SYS$OUTPUT "Checking for TIFF library ...   No"
$       WRITE SYS$OUTPUT "No TIFF library installed. This is fatal. Please download and install good library from fafner.dyndns.org/~alexey/libsdl/public.html"
$       GOTO EXIT
$ENDIF
$!
$!
$!WRITING BUILD FILES
$!
$IF (SHARED.EQ.64)
$ THEN
$	COPY SYS$INPUT [.SRC]LIBGD$DEF.OPT
SYMBOL_VECTOR= (-
gdAlphaBlend=PROCEDURE,-
gdImageCreate=PROCEDURE,-
gdImageCreateTrueColor=PROCEDURE,-
gdImageCreateFromPng=PROCEDURE,-
gdImageCreateFromPngCtx=PROCEDURE,-
gdImageCreateFromPngPtr=PROCEDURE,-
gdImageCreateFromGif=PROCEDURE,-
gdImageCreateFromGifCtx=PROCEDURE,-
gdImageCreateFromGifPtr=PROCEDURE,-
gdImageCreateFromWBMP=PROCEDURE,-
gdImageCreateFromWBMPCtx=PROCEDURE,-
gdImageCreateFromWBMPPtr=PROCEDURE,-
gdImageCreateFromJpeg=PROCEDURE,-
gdImageCreateFromJpegCtx=PROCEDURE,-
gdImageCreateFromJpegPtr=PROCEDURE,-
gdImageCreateFromPngSource=PROCEDURE,-
gdImageCreateFromGd=PROCEDURE,-
gdImageCreateFromGdCtx=PROCEDURE,-
gdImageCreateFromGdPtr=PROCEDURE,-
gdImageCreateFromGd2=PROCEDURE,-
gdImageCreateFromGd2Ctx=PROCEDURE,-
gdImageCreateFromGd2Ptr=PROCEDURE,-
gdImageCreateFromGd2Part=PROCEDURE,-
gdImageCreateFromGd2PartCtx=PROCEDURE,-
gdImageCreateFromGd2PartPtr=PROCEDURE,-
gdImageCreateFromXbm=PROCEDURE,-
gdImageCreateFromXpm=PROCEDURE,-
gdImageStringFTCircle=PROCEDURE,-
gdImageDestroy=PROCEDURE,-
gdImageSetPixel=PROCEDURE,-
gdImageGetPixel=PROCEDURE,-
gdImageGetTrueColorPixel=PROCEDURE,-
gdImageAABlend=PROCEDURE,-
gdImageLine=PROCEDURE,-
gdImageDashedLine=PROCEDURE,-
gdImageRectangle=PROCEDURE,-
gdImageFilledRectangle=PROCEDURE,-
gdImageSetClip=PROCEDURE,-
gdImageGetClip=PROCEDURE,-
gdImageBoundsSafe=PROCEDURE,-
gdImageChar=PROCEDURE,-
gdImageCharUp=PROCEDURE,-
gdImageString=PROCEDURE,-
gdImageStringUp=PROCEDURE,-
gdImageString16=PROCEDURE,-
gdImageStringUp16=PROCEDURE,-
gdFontCacheSetup=PROCEDURE,-
gdFontCacheShutdown=PROCEDURE,-
gdFreeFontCache=PROCEDURE,-
gdImageStringTTF=PROCEDURE,-
gdImageStringFT=PROCEDURE,-
gdFTUseFontConfig=PROCEDURE,-
gdImageStringFTEx=PROCEDURE,-
gdImagePolygon=PROCEDURE,-
gdImageOpenPolygon=PROCEDURE,-
gdImageFilledPolygon=PROCEDURE,-
gdImageColorAllocate=PROCEDURE,-
gdImageColorAllocateAlpha=PROCEDURE,-
gdImageColorClosest=PROCEDURE,-
gdImageColorClosestAlpha=PROCEDURE,-
gdImageColorClosestHWB=PROCEDURE,-
gdImageColorExact=PROCEDURE,-
gdImageColorExactAlpha=PROCEDURE,-
gdImageColorResolve=PROCEDURE,-
gdImageColorResolveAlpha=PROCEDURE,-
gdImageColorDeallocate=PROCEDURE,-
gdImageCreatePaletteFromTrueCol=PROCEDURE,-
gdImageTrueColorToPalette=PROCEDURE,-
gdImageColorTransparent=PROCEDURE,-
gdImagePaletteCopy=PROCEDURE,-
gdImageGif=PROCEDURE,-
gdImagePng=PROCEDURE,-
gdImagePngCtx=PROCEDURE,-
gdImageGifCtx=PROCEDURE,-
gdImagePngEx=PROCEDURE,-
gdImagePngCtxEx=PROCEDURE,-
gdImageWBMP=PROCEDURE,-
gdImageWBMPCtx=PROCEDURE,-
gdFree=PROCEDURE,-
gdImageWBMPPtr=PROCEDURE,-
gdImageJpeg=PROCEDURE,-
gdImageJpegCtx=PROCEDURE,-
gdImageJpegPtr=PROCEDURE,-
gdImageGifAnimBegin=PROCEDURE,-
gdImageGifAnimAdd=PROCEDURE,-
gdImageGifAnimEnd=PROCEDURE,-
gdImageGifAnimBeginCtx=PROCEDURE,-
gdImageGifAnimAddCtx=PROCEDURE,-
gdImageGifAnimEndCtx=PROCEDURE,-
gdImageGifAnimBeginPtr=PROCEDURE,-
gdImageGifAnimAddPtr=PROCEDURE,-
gdImageGifAnimEndPtr=PROCEDURE,-
gdImagePngToSink=PROCEDURE,-
gdImageGd=PROCEDURE,-
gdImageGd2=PROCEDURE,-
gdImageGifPtr=PROCEDURE,-
gdImagePngPtr=PROCEDURE,-
gdImagePngPtrEx=PROCEDURE,-
gdImageGdPtr=PROCEDURE,-
gdImageGd2Ptr=PROCEDURE,-
gdImageFilledArc=PROCEDURE,-
gdImageArc=PROCEDURE,-
gdImageFilledEllipse=PROCEDURE,-
gdImageFillToBorder=PROCEDURE,-
gdImageFill=PROCEDURE,-
gdImageCopy=PROCEDURE,-
gdImageCopyMerge=PROCEDURE,-
gdImageCopyMergeGray=PROCEDURE,-
gdImageCopyResized=PROCEDURE,-
gdImageCopyResampled=PROCEDURE,-
gdImageCopyRotated=PROCEDURE,-
gdImageSetBrush=PROCEDURE,-
gdImageSetTile=PROCEDURE,-
gdImageSetAntiAliased=PROCEDURE,-
gdImageSetAntiAliasedDontBlend=PROCEDURE,-
gdImageSetStyle=PROCEDURE,-
gdImageSetThickness=PROCEDURE,-
gdImageInterlace=PROCEDURE,-
gdImageAlphaBlending=PROCEDURE,-
gdImageSaveAlpha=PROCEDURE,-
gdNewFileCtx=PROCEDURE,-
gdNewDynamicCtx=PROCEDURE,-
gdNewDynamicCtxEx=PROCEDURE,-
gdNewSSCtx=PROCEDURE,-
gdDPExtractData=PROCEDURE,-
gdFontGetGiant=PROCEDURE,-
gdFontGetLarge=PROCEDURE,-
gdFontGetMediumBold=PROCEDURE,-
gdFontGetSmall=PROCEDURE,-
gdFontGetTiny=PROCEDURE,-
gdImageCompare=PROCEDURE,-
gdImageCreateFromTiff=PROCEDURE,-
gdImageCreateFromTiffCtx=PROCEDURE,-
gdImageCreateFromTiffPtr=PROCEDURE,-
gdImageCreateFromTga=PROCEDURE,-
gdImageCreateFromTgaCtx=PROCEDURE,-
gdImageCreateFromTgaPtr=PROCEDURE,-
gdImageTiff=PROCEDURE,-
gdImageTiffPtr=PROCEDURE,-
gdImageTiffCtx=PROCEDURE,-
gdImageCrop=PROCEDURE,-
gdImageAutoCrop=PROCEDURE,-
gdImageThresholdCrop=PROCEDURE,-
gdFontGiant=DATA,-
GDFONTLARGE=DATA,-
GDFONTMEDIUMBOLD=DATA,-
GDFONTSMALL=DATA,-
GDFONTTINY=DATA-
)
$ENDIF
$!
$IF (SHARED.EQ.32)
$ THEN
$	COPY SYS$INPUT [.SRC]LIBGD$DEF.OPT
UNIVERSAL=gdAlphaBlend
UNIVERSAL=gdImageCreate
UNIVERSAL=gdImageCreateTrueColor
UNIVERSAL=gdImageCreateFromPng
UNIVERSAL=gdImageCreateFromPngCtx
UNIVERSAL=gdImageCreateFromPngPtr
UNIVERSAL=gdImageCreateFromGif
UNIVERSAL=gdImageCreateFromGifCtx
UNIVERSAL=gdImageCreateFromGifPtr
UNIVERSAL=gdImageCreateFromWBMP
UNIVERSAL=gdImageCreateFromWBMPCtx
UNIVERSAL=gdImageCreateFromWBMPPtr
UNIVERSAL=gdImageCreateFromJpeg
UNIVERSAL=gdImageStringFTCircle
UNIVERSAL=gdImageCreateFromJpegCtx
UNIVERSAL=gdImageCreateFromJpegPtr
UNIVERSAL=gdImageCreateFromPngSource
UNIVERSAL=gdImageCreateFromGd
UNIVERSAL=gdImageCreateFromGdCtx
UNIVERSAL=gdImageCreateFromGdPtr
UNIVERSAL=gdImageCreateFromGd2
UNIVERSAL=gdImageCreateFromGd2Ctx
UNIVERSAL=gdImageCreateFromGd2Ptr
UNIVERSAL=gdImageCreateFromGd2Part
UNIVERSAL=gdImageCreateFromGd2PartCtx
UNIVERSAL=gdImageCreateFromGd2PartPtr
UNIVERSAL=gdImageCreateFromXbm
UNIVERSAL=gdImageCreateFromXpm
UNIVERSAL=gdImageDestroy
UNIVERSAL=gdImageSetPixel
UNIVERSAL=gdImageGetPixel
UNIVERSAL=gdImageGetTrueColorPixel
UNIVERSAL=gdImageAABlend
UNIVERSAL=gdImageLine
UNIVERSAL=gdImageDashedLine
UNIVERSAL=gdImageRectangle
UNIVERSAL=gdImageFilledRectangle
UNIVERSAL=gdImageSetClip
UNIVERSAL=gdImageGetClip
UNIVERSAL=gdImageBoundsSafe
UNIVERSAL=gdImageChar
UNIVERSAL=gdImageCharUp
UNIVERSAL=gdImageString
UNIVERSAL=gdImageStringUp
UNIVERSAL=gdImageString16
UNIVERSAL=gdImageStringUp16
UNIVERSAL=gdFontCacheSetup
UNIVERSAL=gdFontCacheShutdown
UNIVERSAL=gdFreeFontCache
UNIVERSAL=gdImageStringTTF
UNIVERSAL=gdImageStringFT
UNIVERSAL=gdFTUseFontConfig
UNIVERSAL=gdImageStringFTEx
UNIVERSAL=gdImagePolygon
UNIVERSAL=gdImageOpenPolygon
UNIVERSAL=gdImageFilledPolygon
UNIVERSAL=gdImageColorAllocate
UNIVERSAL=gdImageColorAllocateAlpha
UNIVERSAL=gdImageColorClosest
UNIVERSAL=gdImageColorClosestAlpha
UNIVERSAL=gdImageColorClosestHWB
UNIVERSAL=gdImageColorExact
UNIVERSAL=gdImageColorExactAlpha
UNIVERSAL=gdImageColorResolve
UNIVERSAL=gdImageColorResolveAlpha
UNIVERSAL=gdImageColorDeallocate
UNIVERSAL=gdImageCreatePaletteFromTrueCol
UNIVERSAL=gdImageTrueColorToPalette
UNIVERSAL=gdImageColorTransparent
UNIVERSAL=gdImagePaletteCopy
UNIVERSAL=gdImageGif
UNIVERSAL=gdImagePng
UNIVERSAL=gdImagePngCtx
UNIVERSAL=gdImageGifCtx
UNIVERSAL=gdImagePngEx
UNIVERSAL=gdImagePngCtxEx
UNIVERSAL=gdImageWBMP
UNIVERSAL=gdImageWBMPCtx
UNIVERSAL=gdFree
UNIVERSAL=gdImageWBMPPtr
UNIVERSAL=gdImageJpeg
UNIVERSAL=gdImageJpegCtx
UNIVERSAL=gdImageJpegPtr
UNIVERSAL=gdImageGifAnimBegin
UNIVERSAL=gdImageGifAnimAdd
UNIVERSAL=gdImageGifAnimEnd
UNIVERSAL=gdImageGifAnimBeginCtx
UNIVERSAL=gdImageGifAnimAddCtx
UNIVERSAL=gdImageGifAnimEndCtx
UNIVERSAL=gdImageGifAnimBeginPtr
UNIVERSAL=gdImageGifAnimAddPtr
UNIVERSAL=gdImageGifAnimEndPtr
UNIVERSAL=gdImagePngToSink
UNIVERSAL=gdImageGd
UNIVERSAL=gdImageGd2
UNIVERSAL=gdImageGifPtr
UNIVERSAL=gdImagePngPtr
UNIVERSAL=gdImagePngPtrEx
UNIVERSAL=gdImageGdPtr
UNIVERSAL=gdImageGd2Ptr
UNIVERSAL=gdImageFilledArc
UNIVERSAL=gdImageArc
UNIVERSAL=gdImageFilledEllipse
UNIVERSAL=gdImageFillToBorder
UNIVERSAL=gdImageFill
UNIVERSAL=gdImageCopy
UNIVERSAL=gdImageCopyMerge
UNIVERSAL=gdImageCopyMergeGray
UNIVERSAL=gdImageCopyResized
UNIVERSAL=gdImageCopyResampled
UNIVERSAL=gdImageCopyRotated
UNIVERSAL=gdImageSetBrush
UNIVERSAL=gdImageSetTile
UNIVERSAL=gdImageSetAntiAliased
UNIVERSAL=gdImageSetAntiAliasedDontBlend
UNIVERSAL=gdImageSetStyle
UNIVERSAL=gdImageSetThickness
UNIVERSAL=gdImageInterlace
UNIVERSAL=gdImageAlphaBlending
UNIVERSAL=gdImageSaveAlpha
UNIVERSAL=gdNewFileCtx
UNIVERSAL=gdNewDynamicCtx
UNIVERSAL=gdNewDynamicCtxEx
UNIVERSAL=gdNewSSCtx
UNIVERSAL=gdDPExtractData
UNIVERSAL=gdFontGetGiant
UNIVERSAL=gdFontGetLarge
UNIVERSAL=gdFontGetMediumBold
UNIVERSAL=gdFontGetSmall
UNIVERSAL=gdFontGetTiny
UNIVERSAL=gdImageCompare
UNIVERSAL=gdFontGiant
UNIVERSAL=gdImageCreateFromTiff
UNIVERSAL=gdImageCreateFromTiffCtx
UNIVERSAL=gdImageCreateFromTiffPtr
UNIVERSAL=gdImageCreateFromTga
UNIVERSAL=gdImageCreateFromTgaCtx
UNIVERSAL=gdImageCreateFromTgaPtr
UNIVERSAL=gdImageTiff
UNIVERSAL=gdImageTiffPtr
UNIVERSAL=gdImageTiffCtx
UNIVERSAL=gdImageCrop
UNIVERSAL=gdImageAutoCrop
UNIVERSAL=gdImageThresholdCrop
UNIVERSAL=GDFONTLARGE
UNIVERSAL=GDFONTMEDIUMBOLD
UNIVERSAL=GDFONTSMALL
UNIVERSAL=GDFONTTINY

$ENDIF
$!
$!
$DEF SYS$OUTPUT _NL0:
$!
$!Making OPT file for GD$SHR.EXE, i.e without reference to himself
$COPY SYS$INPUT [.SRC]OPT.OPT
[]GD/LIB
$IF ((HAVE_LIBPNG.EQ.1) .AND. (LIBPNG_STATIC.EQ.1))
$  THEN
$       APP SYS$INPUT [.SRC]OPT.OPT
PNG:LIBPNG/LIB
$ENDIF
$IF ((HAVE_LIBPNG.EQ.1) .AND. (LIBPNG_STATIC.EQ.0))
$  THEN
$       APP PNG:LIBPNG$SHR.OPT [.SRC]OPT.OPT
$ENDIF
$!
$IF ((HAVE_JPEG.EQ.1) .AND. (JPEG_STATIC.EQ.1) .AND. (HAVE_TIFF.NE.1))
$  THEN
$       APP SYS$INPUT [.SRC]OPT.OPT
JPEG:LIBJPEG/LIB
$ENDIF
$IF ((HAVE_JPEG.EQ.1) .AND. (JPEG_STATIC.EQ.0) .AND. (HAVE_TIFF.NE.1))
$  THEN
$       APP JPEG:LIBJPEG$SHR.OPT [.SRC]OPT.OPT
$ENDIF
$!
$IF (HAVE_TIFF.EQ.1)
$  THEN
$       APP TIFF:LIBTIFF.OPT [.SRC]OPT.OPT
$ENDIF
$!
$IF (HAVE_FREETYPE.EQ.1) .AND. (FREETYPE_STATIC.EQ.1)
$ THEN
$       APP SYS$INPUT [.SRC]OPT.OPT
        FREETYPE2:FREETYPE/LIB
        ZLIB:LIBZ/LIB
$ENDIF
$IF (HAVE_FREETYPE.EQ.1) .AND. (FREETYPE_STATIC.EQ.0)
$ THEN
$       APP FREETYPE2:FREETYPE.OPT [.SRC]OPT.OPT
$ENDIF
$!
$IF (HAVE_ZLIB.EQ.1) .AND. ((FREETYPE_STATIC.NE.1).AND.(JPEG_STATIC.NE.1))
$  THEN
$       IF (ZLIB_STATIC.EQ.0)
$         THEN
$               APP SYS$INPUT [.SRC]OPT.OPT
                SYS$SHARE:LIBZSHR/SHARE
$         ELSE
$               APP SYS$INPUT [.SRC]OPT.OPT
                ZLIB:LIBZ/LIB
$       ENDIF
$  ELSE
$	APP SYS$INPUT [.SRC]OPT.OPT
	ZLIB:LIBZ/LIB
$ENDIF
$!
$!
$PURGE [.SRC]OPT.OPT
$!
$!
$!Making working LIBGD.OPT file
$!
$IF (SHARED.GT.0)
$ THEN
	$COPY SYS$INPUT [.SRC]LIBGD.OPT
	SYS$SHARE:GD$SHR/SHARE
$ ELSE
	$COPY SYS$INPUT [.SRC]LIBGD.OPT
	LIBGD:GD/LIB
$ENDIF
$!
$!Append OPT files to LIBGD.OPT
$IF ((HAVE_LIBPNG.EQ.1) .AND. (LIBPNG_STATIC.EQ.1))
$  THEN
$	APP SYS$INPUT [.SRC]LIBGD.OPT
PNG:LIBPNG/LIB
$ENDIF
$IF ((HAVE_LIBPNG.EQ.1) .AND. (LIBPNG_STATIC.EQ.0))
$  THEN
$	APP PNG:LIBPNG$SHR.OPT [.SRC]LIBGD.OPT
$ENDIF
$!
$IF ((HAVE_JPEG.EQ.1) .AND. (HAVE_TIFF.NE.1) .AND. (JPEG_STATIC.EQ.1))
$  THEN
$       APP JPEG:LIBJPEG.OPT [.SRC]LIBGD.OPT
$ENDIF
$IF ((HAVE_JPEG.EQ.1) .AND. (JPEG_STATIC.EQ.0) .AND. (HAVE_TIFF.NE.1))
$  THEN
$       APP JPEG:LIBJPEG$SHR.OPT [.SRC]LIBGD.OPT
$ENDIF
$!
$IF (HAVE_TIFF.EQ.1)
$  THEN
$	APP TIFF:LIBTIFF.OPT [.SRC]LIBGD.OPT
$ENDIF
$!
$IF (HAVE_FREETYPE.EQ.1) .AND. (FREETYPE_STATIC.EQ.1)
$ THEN
$	APP SYS$INPUT [.SRC]LIBGD.OPT
	FREETYPE2:FREETYPE/LIB
	ZLIB:LIBZ/LIB
$ENDIF
$IF (HAVE_FREETYPE.EQ.1) .AND. (FREETYPE_STATIC.EQ.0)
$ THEN
$	APP FREETYPE2:FREETYPE.OPT [.SRC]LIBGD.OPT
$ENDIF
$!
$IF (HAVE_ZLIB.EQ.1) .AND. ((FREETYPE_STATIC.NE.1).AND.(JPEG_STATIC.NE.1))
$  THEN
$	IF (ZLIB_STATIC.EQ.0)
$	  THEN
$		APP SYS$INPUT [.SRC]LIBGD.OPT
		SYS$SHARE:LIBZSHR/SHARE
$	  ELSE
$		APP SYS$INPUT [.SRC]LIBGD.OPT
		ZLIB:LIBZ/LIB
$	ENDIF
$ENDIF
$!
$PURGE LIBGD.OPT
$!
$DEAS SYS$OUTPUT
$!
$OPEN/WRITE OUT BUILD.COM
$ WRITE OUT "$SET DEF [.SRC]"
$ WRITE OUT "$",MAKE
$ WRITE OUT "$ CURRENT = F$ENVIRONMENT (""DEFAULT"") "
$ WRITE OUT "$TMP=CURRENT"
$ WRITE OUT "$OPEN/WRITE OUTT [-]LIBGD$STARTUP.COM"
$ WRITE OUT "$WRITE OUTT ""DEFINE LIBGD """"", "'","'","TMP'"""""""
$ WRITE OUT "$WRITE OUTT ""GD2COPYPAL==""""$","'","'","TMP'"",""GD2COPYPAL"""" "
$ WRITE OUT "$WRITE OUTT ""GD2TIME==""""$","'","'","TMP'"",""GD2TIME"""" "
$ WRITE OUT "$WRITE OUTT ""GD2TOGIF==""""$","'","'","TMP'"",""GD2TOGIF"""" "
$ WRITE OUT "$WRITE OUTT ""GD2TOPNG==""""$","'","'","TMP'"",""GD2TOPNG"""" "
$ WRITE OUT "$WRITE OUTT ""GDCMPGIF==""""$","'","'","TMP'"",""GDCMPGIF"""" "
$ WRITE OUT "$WRITE OUTT ""GDTOPNG==""""$","'","'","TMP'"",""GDTOPNG"""" "
$ WRITE OUT "$WRITE OUTT ""GIFTOGD2==""""$","'","'","TMP'"",""GIFTOGD2"""" "
$ WRITE OUT "$WRITE OUTT ""PNGTOGD==""""$","'","'","TMP'"",""PNGTOGD"""" "
$ WRITE OUT "$WRITE OUTT ""PNGTOGD2==""""$","'","'","TMP'"",""PNGTOGD2"""" "
$ WRITE OUT "$WRITE OUTT ""WEBPNG==""""$","'","'","TMP'"",""WEBPNG"""" "
$ WRITE OUT "$CLOSE OUTT"
$!
$ WRITE OUT "$WRITE SYS$OUTPUT "" "" "
$ WRITE OUT "$WRITE SYS$OUTPUT ""***************************************************************************** "" "
$ WRITE OUT "$WRITE SYS$OUTPUT ""LIBGD$STARTUP.COM has been created. "" "
$ WRITE OUT "$WRITE SYS$OUTPUT ""This file setups all logicals needed. It should be execute before using "" "
$ WRITE OUT "$WRITE SYS$OUTPUT ""Nice place to call it - LOGIN.COM "" "
$ WRITE OUT "$WRITE SYS$OUTPUT """" "
$ WRITE OUT "$WRITE SYS$OUTPUT ""Using:"" "
$ WRITE OUT "$WRITE SYS$OUTPUT ""CC/INCL=LIBGD PROG"" "
$ WRITE OUT "$WRITE SYS$OUTPUT ""LINK PROG, LIBGD:LIBGD/OPT"" "
$ WRITE OUT "$WRITE SYS$OUTPUT ""***************************************************************************** "" "
$ WRITE OUT "$WRITE SYS$OUTPUT ""***************************************************************************** "" "
$ WRITE OUT "$SET DEF [-]"
$CLOSE OUT
$!
$!
$!Writing config.h
$COPY SYS$INPUT [.SRC]CONFIG.H
/* Generated by configure.com  for OpenVMS */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1


#define PACKAGE_NAME "GD"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "GD 2.1.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "GD"

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.1.0"

/* Define to necessary symbol if this constant uses a non-standard name on
 *    your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "2.1.0"

/* Define to 1 if you have the `m' library (-lm). */
#define HAVE_LIBM 1


/* Define to 1 if you have the <libpng/png.h> header file. */
/* #undef HAVE_LIBPNG_PNG_H */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1


/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1

/* Define to 1 if you have the <stdint.h> header file. */
//#define HAVE_STDINT_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define as const if the declaration of iconv() needs const. */
//#define ICONV_CONST

/* Name of package */
#define PACKAGE "gd"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://bugs.libgd.org"

/* Define to the full name of this package. */



$IF HAVE_ICONV.EQ.1
$ THEN
$	APP SYS$INPUT [.SRC]CONFIG.H
	/* Define if you have the iconv() function. */
	#define HAVE_ICONV 1
	/* Define to 1 if you have the <iconv.h> header file. */
	#define HAVE_ICONV_H 1
	/* Define if <iconv.h> defines iconv_t. */
	#define HAVE_ICONV_T_DEF 1
$ENDIF
$IF HAVE_TIFF.EQ.1
$  THEN
$	APP SYS$INPUT [.SRC]CONFIG.H
	#define HAVE_LIBTIFF 1
$ENDIF
$IF HAVE_LIBPNG.EQ.1
$   THEN
$	APP SYS$INPUT [.SRC]CONFIG.H
	#define HAVE_LIBPNG 1
$ENDIF
$IF HAVE_FREETYPE.EQ.1
$   THEN
$	APP SYS$INPUT [.SRC]CONFIG.H
	#define HAVE_LIBFREETYPE 1
	#define HAVE_FT2BUILD_H 1
$ENDIF
$IF HAVE_JPEG.EQ.1
$   THEN
$	APP SYS$INPUT [.SRC]CONFIG.H
	#define HAVE_LIBJPEG 1
$ENDIF
$IF HAVE_ZLIB.EQ.1
$   THEN
$	APP SYS$INPUT [.SRC]CONFIG.H
	#define HAVE_LIBZ 1
$ENDIF
$IF HAVE_TIFF.EQ.1
$   THEN
$	APP SYS$INPUT [.SRC]CONFIG.H
	#define HAVE_LIBTIFF 1
$ENDIF
$!
$!
$!
$!
$COP SYS$INPUT [.SRC]DESCRIP.MMS
# Generated by CONFIGURE.COM
# Alexey Chupahin elvis_75@mail.ru
# OpenVMS 7.3-1,8.2  DEC 2000 mod.300
# OpenVMS 8.3        HP  rx1620


.FIRST
	DEF LIBGD []

CC=cc
CFLAGS=/OPT=(INLINE=SPEED) /INCL=([],JPEG,PNG,FREETYPE2,ZLIB,TIFF) /DEF=(HAVE_CONFIG_H)/WARN=(DIS=MACROREDEF)
LIBS=[]LIBGD/OPT


ALL : \
gd.olb,\
$IF (SHARED.GT.0)
$ THEN
$	APP SYS$INPUT [.SRC]DESCRIP.MMS
gd$shr.exe,\
$ENDIF
$!
$APP SYS$INPUT [.SRC]DESCRIP.MMS
annotate.exe,\
circletexttest.exe,\
fontconfigtest.exe,\
circletexttest.exe,\
fontconfigtest.exe,\
fontsizetest.exe,\
fontwheeltest.exe,\
gd2copypal.exe,\
gd2time.exe,\
gd2togif.exe,\
gd2topng.exe,\
gdcmpgif.exe,\
gddemo.exe,\
gdtest.exe,\
gdtestft.exe,\
gdtopng.exe,\
gifanimtest.exe,\
giftogd2.exe,\
pngtogd.exe,\
pngtogd2.exe,\
testac.exe,\
testtr.exe,\
webpng.exe,\
gdparttopng.exe
	$!

annotate.exe : annotate.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*

circletexttest.exe : circletexttest.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*

fontconfigtest.exe : fontconfigtest.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*

fontsizetest.exe : fontsizetest.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*

fontwheeltest.exe : fontwheeltest.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*

gd2copypal.exe : gd2copypal.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*

gd2time.exe : gd2time.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*

gd2togif.exe : gd2togif.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*

gd2topng.exe : gd2topng.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*


gdcmpgif.exe : gdcmpgif.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*


gddemo.exe : gddemo.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*


gdtest.exe : gdtest.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*


gdtestft.exe : gdtestft.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*


gdtopng.exe : gdtopng.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*


gifanimtest.exe : gifanimtest.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*

giftogd2.exe : giftogd2.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*

giftogd2.obj : giftogd2.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*


pngtogd.exe : pngtogd.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*


pngtogd2.exe : pngtogd2.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*


testac.exe : testac.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*


testtr.exe : testtr.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*


webpng.exe : webpng.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*


gdparttopng.exe : gdparttopng.c
       $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=TMP.OBJ
       LINK/EXE=$(MMS$TARGET)  TMP.OBJ, $(LIBS)
       DEL TMP.OBJ;*


OBJ=\
gd.obj,\
gdcache.obj,\
gdfontg.obj,\
gdfontl.obj,\
gdfontmb.obj,\
gdfonts.obj,\
gdfontt.obj,\
gdft.obj,\
gdfx.obj,\
gdhelpers.obj,\
gdkanji.obj,\
gdtables.obj,\
gdxpm.obj,\
gd_bmp.obj,\
gd_gd.obj,\
gd_gd2.obj,\
gd_gif_in.obj,\
gd_gif_out.obj,\
gd_io.obj,\
gd_io_dp.obj,\
gd_io_file.obj,\
gd_io_ss.obj,\
gd_jpeg.obj,\
gd_png.obj,\
gd_security.obj,\
gd_ss.obj,\
gd_crop.obj,\
gd_tga.obj,\
gd_tiff.obj,\
gd_transform.obj,\
gd_topal.obj,\
gd_wbmp.obj,\
wbmp.obj


gd_transform.obj : gd_transform.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_tiff.obj : gd_tiff.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_tga.obj : gd_tga.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_bmp.obj : gd_bmp.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_crop.obj : gd_crop.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd.obj : gd.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gdcache.obj : gdcache.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gdfontg.obj : gdfontg.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gdfontl.obj : gdfontl.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gdfontmb.obj : gdfontmb.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gdfonts.obj : gdfonts.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gdfontt.obj : gdfontt.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gdft.obj : gdft.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gdfx.obj : gdfx.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gdhelpers.obj : gdhelpers.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gdkanji.obj : gdkanji.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gdtables.obj : gdtables.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gdxpm.obj : gdxpm.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_gd.obj : gd_gd.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_gd2.obj : gd_gd2.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_gif_in.obj : gd_gif_in.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_gif_out.obj : gd_gif_out.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_io.obj : gd_io.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_io_dp.obj : gd_io_dp.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_io_file.obj : gd_io_file.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_io_ss.obj : gd_io_ss.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_jpeg.obj : gd_jpeg.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_png.obj : gd_png.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_security.obj : gd_security.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_ss.obj : gd_ss.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_topal.obj : gd_topal.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

gd_wbmp.obj : gd_wbmp.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)

wbmp.obj : wbmp.c
         $(CC) $(CFLAGS) $(MMS$SOURCE) /OBJ=$(MMS$TARGET)


GD.OLB : $(OBJ)
        LIB/CREA GD.OLB $(OBJ)

GD$SHR.EXE : $(OBJ)
	LINK/SHARE=GD$SHR GD.OBJ,LIBGD$DEF/OPT,OPT/OPT
	COPY GD$SHR.EXE SYS$SHARE

CLEAN :
	DEL [...]*.OBJ;*
	DEL [...]*.OLB;*
	DEL [...]*.EXE;*
$!DEL TMP.MMS;*
$!
$IF (F$SEARCH("[.SRC]GD.H;2").EQS."")
$ THEN
$       WRITE SYS$OUTPUT "Patching GD.H"
$	DEF SYS$OUTPUT "_NLA0:"
$	COPY SYS$INPUT [.SRC]GD.H;2
#define gdImageCreatePaletteFromTrueColor gdImageCreatePaletteFromTrueCol

$       APP [.SRC]GD.H;1 [.SRC]GD.H;2
$	DEAS SYS$INPUT
$ENDIF
$!
$!
$!
$!
$WRITE SYS$OUTPUT " "
$WRITE SYS$OUTPUT " "
$WRITE SYS$OUTPUT "Now you can type @BUILD "
$!
$EXIT:
$DEFINE SYS$OUTPUT _NLA0:
$DEL TEST.OBJ;*
$DEL TEST.C;*
$DEL TEST.EXE;*
$DEAS SYS$OUTPUT
