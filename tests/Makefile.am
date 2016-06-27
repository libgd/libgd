check_LIBRARIES = libgdtest.a

libgdtest_a_SOURCES = \
	gdtest/gdtest.c \
	gdtest/gdtest.h \
	gdhelpers.c

# Setup vars that subdirs will increment.
libgd_test_programs =
libgd_helper_programs =
CLEANFILES =
EXTRA_DIST =
TESTS =

include bmp/Makemodule.am
include freetype/Makemodule.am
include gd/Makemodule.am
include gd2/Makemodule.am
include gdimagearc/Makemodule.am
include gdimagecolorclosest/Makemodule.am
include gdimagecolordeallocate/Makemodule.am
include gdimagecolorexact/Makemodule.am
include gdimagecolorreplace/Makemodule.am
include gdimagecolorresolve/Makemodule.am
include gdimagecolortransparent/Makemodule.am
include gdimagecopy/Makemodule.am
include gdimagecopyrotated/Makemodule.am
include gdimagecrop/Makemodule.am
include gdimagefile/Makemodule.am
include gdimagefill/Makemodule.am
include gdimagefilledellipse/Makemodule.am
include gdimagefilledpolygon/Makemodule.am
include gdimagefilledrectangle/Makemodule.am
include gdimagefilltoborder/Makemodule.am
include gdimagefilter/Makemodule.am
include gdimageline/Makemodule.am
include gdimageopenpolygon/Makemodule.am
include gdimagepixelate/Makemodule.am
include gdimagepolygon/Makemodule.am
include gdimagerectangle/Makemodule.am
include gdimagerotate/Makemodule.am
include gdimagescale/Makemodule.am
include gdimagescatterex/Makemodule.am
include gdimagesetpixel/Makemodule.am
include gdimagestringft/Makemodule.am
include gdimagestringftex/Makemodule.am
include gdinterpolatedscale/Makemodule.am
include gdnewfilectx/Makemodule.am
include gdtest/Makemodule.am
include gdtiled/Makemodule.am
include gdtransformaffineboundingbox/Makemodule.am
include gif/Makemodule.am
include jpeg/Makemodule.am
include png/Makemodule.am
include tga/Makemodule.am
include tiff/Makemodule.am
include wbmp/Makemodule.am
include webp/Makemodule.am
include xbm/Makemodule.am
include xpm/Makemodule.am

LDADD = libgdtest.a ../src/libgd.la
AM_CPPFLAGS = -I$(top_srcdir)/src -I $(top_srcdir)/tests/gdtest

check_PROGRAMS = $(libgd_test_programs) $(libgd_helper_programs)
TESTS += $(libgd_test_programs)

EXTRA_DIST += \
	CMakeLists.txt

# We don't keep any media files in the top dir ... just generated outputs.
CLEANFILES += \
	*.bmp \
	*.gd \
	*.gd2 \
	*.gif \
	*.jpeg \
	*.jpg \
	*.png \
	*.tga \
	*.tiff \
	*.ttf \
	*.wbmp \
	*.webp \
	*.xbm \
	*.xpm
