noinst_LIBRARIES = libgdtest.a

libgdtest_a_SOURCES = \
	gdtest/gdtest.c \
	gdtest/gdtest.h \
	../src/gdhelpers.c

check_PROGRAMS = \
	gd2/gd2_empty_file \
	gdimagesetpixel/bug00186 \
	gdimagestringftex/gdimagestringftex_returnfontpathname \
	gdimageline/gdimageline_aa_outofrange \
	gdimagerectangle/bug00003 \
	gdimagerectangle/bug00106_gdimagerectangle \
	gd/gd_null \
	gd/gd_num_colors \
	tga/tga_null \
	gdimagepixelate/gdimagepixelate \
	gdimagecopy/bug00007 \
	gdnewfilectx/gdnewfilectx_null \
	gdimagefilledrectangle/bug00078 \
	gdimagefilledrectangle/bug00004 \
	gdimagefilledrectangle/bug00106_gdimagefilledrectangle \
	gdimagecolordeallocate/gdimagecolordeallocate \
	wbmp/wbmp_null \
	gdimagecolortransparent/gdimagecolortransparent \
	gif/bug00005_2 \
	gif/gif_null \
	gif/bug00181 \
	gif/bug00227 \
	bmp/bmp_null

EXTRA_PROGRAMS = \
	gdimagestringft/gdimagestringft_bbox \
	gdtiled/bug00032 \
	gd2/gd2_im2im \
	gd2/gd2_null \
	gd2/gd2_read \
	gdimagearc/bug00079 \
	gdimageline/gdimageline_aa \
	gdimageline/bug00072 \
	gdimageline/bug00077 \
	gdimageline/bug00111 \
	gdimageline/gdimageline_bug5 \
	gdimageline/gdImageAALine_thickness \
	gdimageopenpolygon/gdimageopenpolygon2 \
	gdimageopenpolygon/gdimageopenpolygon1 \
	gdimageopenpolygon/gdimageopenpolygon0 \
	gdimageopenpolygon/gdimageopenpolygon3 \
	gd/gd_im2im \
	gdimagecopy/bug00081 \
	gdimagecopyrotated/bug00020 \
	gdimagepolygon/gdimagepolygon0 \
	gdimagepolygon/gdimagepolygon3 \
	gdimagepolygon/gdimagepolygon1 \
	gdimagepolygon/gdimagepolygon2 \
	gdimagecolorresolve/gdimagecolorresolve \
	gdimagefilledpolygon/gdimagefilledpolygon0 \
	gdimagefilledpolygon/gdimagefilledpolygon3 \
	gdimagefilledpolygon/gdimagefilledpolygon2 \
	gdimagefilledpolygon/gdimagefilledpolygon1 \
	gdimagefilledpolygon/bug00100 \
	gdimagecolorreplace/gdimagecolorreplace \
	gdimagefill/bug00002_2 \
	gdimagefill/bug00002_1 \
	gdimagefill/bug00002_4 \
	gdimagefill/bug00002_3 \
	gdimagecolorclosest/gdimagecolorclosest \
	gif/bug00005 \
	gif/gif_im2im \
	gif/bug00066 \
	gif/bug00006 \
	gif/bug00060 \
	gdimagecolorexact/gdimagecolorexact \
	gdimagefilledellipse/bug00191 \
	gdimagefilledellipse/bug00010 \
	xpm/bug00185 \
	xpm/color_name \
	xpm/bug00166 \
	jpeg/jpeg_empty_file \
	jpeg/jpeg_resolution \
	jpeg/jpeg_im2im \
	jpeg/jpeg_null \
	jpeg/jpeg_read \
	png/png_null \
	png/bug00033 \
	png/bug00193 \
	png/bug00011 \
	png/bug00088 \
	png/png_resolution \
	png/bug00086 \
	png/png_im2im \
	wbmp/wbmp_im2im \
	tiff/tiff_null \
	tiff/tiff_im2im \
	freetype/bug00132 \
	bmp/bmp_im2im \
	gdimagescatterex/bug00208_1 \
	gdimagescatterex/bug00208_2 \
	gdimagefilltoborder/bug00037 \
	gdimagerotate/php_bug_64898 \
	gdimagerotate/bug00067

if HAVE_LIBXPM
check_PROGRAMS += \
	xpm/bug00185 \
	xpm/color_name \
	xpm/bug00166
endif

if HAVE_LIBJPEG
check_PROGRAMS += \
	jpeg/jpeg_empty_file \
	jpeg/jpeg_im2im \
	jpeg/jpeg_null
if HAVE_LIBPNG
check_PROGRAMS += \
	jpeg/jpeg_resolution \
	jpeg/jpeg_read
endif
endif

if HAVE_LIBZ
check_PROGRAMS += \
	gd2/gd2_null
endif

if HAVE_LIBPNG
if HAVE_LIBZ
check_PROGRAMS += \
	gd2/gd2_read \
	gd2/gd2_im2im \
	gdtiled/bug00032
endif
check_PROGRAMS += \
	gdimagestringft/gdimagestringft_bbox \
	gdimagearc/bug00079 \
	gdimageline/gdimageline_aa \
	gdimageline/bug00072 \
	gdimageline/bug00077 \
	gdimageline/bug00111 \
	gdimageline/gdimageline_bug5 \
	gdimageline/gdImageAALine_thickness \
	gdimageopenpolygon/gdimageopenpolygon2 \
	gdimageopenpolygon/gdimageopenpolygon1 \
	gdimageopenpolygon/gdimageopenpolygon0 \
	gdimageopenpolygon/gdimageopenpolygon3 \
	gd/gd_im2im \
	gdimagecopy/bug00081 \
	gdimagecopyrotated/bug00020 \
	gdimagepolygon/gdimagepolygon0 \
	gdimagepolygon/gdimagepolygon3 \
	gdimagepolygon/gdimagepolygon1 \
	gdimagepolygon/gdimagepolygon2 \
	gdimagecolorresolve/gdimagecolorresolve \
	gdimagefilledpolygon/gdimagefilledpolygon0 \
	gdimagefilledpolygon/gdimagefilledpolygon3 \
	gdimagefilledpolygon/gdimagefilledpolygon2 \
	gdimagefilledpolygon/gdimagefilledpolygon1 \
	gdimagefilledpolygon/bug00100 \
	gdimagecolorreplace/gdimagecolorreplace \
	gdimagefill/bug00002_2 \
	gdimagefill/bug00002_1 \
	gdimagefill/bug00002_4 \
	gdimagefill/bug00002_3 \
	gdimagecolorclosest/gdimagecolorclosest \
	gif/bug00005 \
	gif/gif_im2im \
	gif/bug00066 \
	gif/bug00006 \
	gif/bug00060 \
	gdimagecolorexact/gdimagecolorexact \
	gdimagefilledellipse/bug00191 \
	gdimagefilledellipse/bug00010 \
	png/png_null \
	png/bug00033 \
	png/bug00193 \
	png/bug00011 \
	png/bug00088 \
	png/png_resolution \
	png/bug00086 \
	png/png_im2im \
	wbmp/wbmp_im2im \
	bmp/bmp_im2im \
	gdimagescatterex/bug00208_1 \
	gdimagescatterex/bug00208_2 \
	gdimagefilltoborder/bug00037 \
	gdimagerotate/php_bug_64898 \
	gdimagerotate/bug00067
endif

if HAVE_LIBTIFF
check_PROGRAMS += \
	tiff/tiff_null \
	tiff/tiff_im2im
endif

if HAVE_LIBFREETYPE
check_PROGRAMS +=
	freetype/bug00132
endif

LDADD = ../src/libgd.la libgdtest.a
AM_CPPFLAGS = -I$(top_srcdir)/src -I $(top_srcdir)/tests/gdtest

TESTS = $(check_PROGRAMS)

EXTRA_DIST = \
	gdtest/test_config.h.cmake \
	xpm/bug00166.xpm \
	xpm/bug00185.xpm \
	xpm/bug00185_damaged.xpm \
	xpm/color_name.xpm \
	gdtiled/bug00032_exp.png \
	jpeg/conv_test.jpeg \
	jpeg/conv_test_exp.png \
	jpeg/empty.jpeg \
	gd2/conv_test.gd2 \
	gd2/conv_gd2_exp.gd2 \
	gd2/empty.gd2 \
	gd2/conv_test_exp.png \
	gdimagearc/bug00079_exp.png \
	png/bug00088_2.png \
	png/bug00088_1_exp.png \
	png/bug00033.png \
	png/bug00088_2_exp.png \
	png/bug00088_1.png \
	png/emptyfile \
	gdimageline/gdimageline_aa_b_0_exp.png \
	gdimageline/gdimageline_aa_c_0_exp.png \
	gdimageline/gdimageline_aa_d_1_exp.png \
	gdimageline/gdimageline_aa_a_1_exp.png \
	gdimageline/gdimageline_aa_d_0_exp.png \
	gdimageline/bug00111_exp.png \
	gdimageline/gdimageline_aa_c_1_exp.png \
	gdimageline/gdimageline_aa_a_0_exp.png \
	gdimageline/gdimageline_aa_b_1_exp.png \
	gdimageline/bug00072_exp.png \
	gdimageline/bug00077_exp.png \
	gdimageopenpolygon/gdimageopenpolygon0.png \
	gdimageopenpolygon/gdimageopenpolygon1.png \
	gdimageopenpolygon/gdimageopenpolygon2.png \
	gdimageopenpolygon/gdimageopenpolygon3.png \
	gd/crafted_num_colors.gd \
	gdimagecopy/bug00081_exp.png \
	gdimagecopyrotated/bug00020_exp.png \
	gdimagepolygon/gdimagepolygon1.png \
	gdimagepolygon/gdimagepolygon3.png \
	gdimagepolygon/gdimagepolygon0.png \
	gdimagepolygon/gdimagepolygon2.png \
	gdimagefilledpolygon/gdimagefilledpolygon0.png \
	gdimagefilledpolygon/gdimagefilledpolygon1.png \
	gdimagefilledpolygon/gdimagefilledpolygon3.png \
	gdimagefilledpolygon/gdimagefilledpolygon2.png \
	gdimagefilledpolygon/bug00100.png \
	gdimagefill/bug00002_3_exp.png \
	gdimagefill/bug00002_2_exp.png \
	gdimagefill/bug00002_1_exp.png \
	gdimagefill/bug00002_4_exp.png \
	gif/bug00005_0.gif \
	gif/bug00005_1.gif \
	gif/bug00005_3.gif \
	gif/bug00005_2.gif \
	gif/bug00060.gif \
	gif/bug00066_exp.png \
	gif/bug00005_2_exp.png \
	gif/bug00066.gif \
	gdimagefilledellipse/bug00191.png \
	gdimagefilledellipse/bug00010_exp.png \
	gdimagescatterex/bug00208_2.png \
	gdimagescatterex/bug00208.png \
	gdimagescatterex/bug00208_1.png \
	freetype/bug00132_exp.png \
	freetype/DejaVuSans.ttf \
	gdimagestringft/CMakeLists.txt \
	gdtest/CMakeLists.txt \
	xpm/CMakeLists.txt \
	gdtiled/CMakeLists.txt \
	jpeg/CMakeLists.txt \
	gd2/CMakeLists.txt \
	gdimagesetpixel/CMakeLists.txt \
	gdimagearc/CMakeLists.txt \
	png/CMakeLists.txt \
	gdimagestringftex/CMakeLists.txt \
	gdimageline/CMakeLists.txt \
	gdimageopenpolygon/CMakeLists.txt \
	gdimagerectangle/CMakeLists.txt \
	gd/CMakeLists.txt \
	tga/CMakeLists.txt \
	gdimagepixelate/CMakeLists.txt \
	tiff/CMakeLists.txt \
	gdimagecopy/CMakeLists.txt \
	CMakeLists.txt \
	gdnewfilectx/CMakeLists.txt \
	gdimagefilledrectangle/CMakeLists.txt \
	gdimagecopyrotated/CMakeLists.txt \
	gdimagepolygon/CMakeLists.txt \
	gdimagecolordeallocate/CMakeLists.txt \
	wbmp/CMakeLists.txt \
	gdimagecolorresolve/CMakeLists.txt \
	gdimagefilledpolygon/CMakeLists.txt \
	gdimagecolorreplace/CMakeLists.txt \
	gdimagefill/CMakeLists.txt \
	gdimagecolorclosest/CMakeLists.txt \
	gdimagecolortransparent/CMakeLists.txt \
	gif/CMakeLists.txt \
	gdimagecolorexact/CMakeLists.txt \
	gdimagefilledellipse/CMakeLists.txt \
	bmp/CMakeLists.txt \
	gdimagescatterex/CMakeLists.txt \
	gdimagefilltoborder/CMakeLists.txt \
	freetype/CMakeLists.txt \
	gdimagerotate/php_bug_64898.png \
	gdimagerotate/php_bug_64898_exp.png \
	gdimagerotate/remirh128.jpg \
	gdimagerotate/bug00067_000_exp.png \
	gdimagerotate/bug00067_015_exp.png \
	gdimagerotate/bug00067_030_exp.png \
	gdimagerotate/bug00067_045_exp.png \
	gdimagerotate/bug00067_060_exp.png \
	gdimagerotate/bug00067_075_exp.png \
	gdimagerotate/bug00067_090_exp.png \
	gdimagerotate/bug00067_105_exp.png \
	gdimagerotate/bug00067_120_exp.png \
	gdimagerotate/bug00067_135_exp.png \
	gdimagerotate/bug00067_150_exp.png \
	gdimagerotate/bug00067_165_exp.png \
	gdimagerotate/bug00067_180_exp.png \
	gdimageline/gdImageAALine_thickness_exp.png

CLEANFILES = \
	a.png \
	bmp_im2im_dst.bmp \
	bmp_im2im_src.bmp \
	bug00132.c_35_diff.png \
	bug00132.c_35_out.png \
	bug00181.gif \
	bug00181a.gif \
	bug00227.gif \
	gd2_im2im_dst.gd2 \
	gd2_im2im_src.gd2 \
	gd_im2im_dst.gd \
	gd_im2im_src.gd \
	gdimageline_aa.c_38_diff.png \
	gdimageline_aa.c_38_out.png \
	gdimagestringft_bbox.png \
	gif_im2im_dst.gif \
	gif_im2im_src.gif \
	jpeg_im2im_dst.jpeg \
	jpeg_im2im_src.jpeg \
	jpeg_read.c_25_diff.png \
	jpeg_read.c_25_out.png \
	png_im2im_dst.png \
	png_im2im_src.png \
	tiff_im2im_dst.tiff \
	tiff_im2im_src.tiff \
	wbmp/wbmp_im2im_dst.wbmp \
	wbmp/wbmp_im2im_src.wbmp \
	wbmp_im2im_dst.wbmp \
	wbmp_im2im_src.wbmp
