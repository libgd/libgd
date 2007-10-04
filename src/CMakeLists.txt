
SET (LIBGD_SRC_FILES
	gd.c
	gdfx.c
	gd_security.c
	gd_gd.c
	gd_gd2.c
	gd_io.c
	gd_io_dp.c
	gd_gif_in.c
	gd_gif_out.c
	gd_io_file.c
	gd_io_ss.c
	gd_jpeg.c
	gd_png.c
	gd_ss.c
	gd_topal.c
	gd_wbmp.c
	gdcache.c
	gdfontg.c
	gdfontl.c
	gdfontmb.c
	gdfonts.c
	gdfontt.c
	gdft.c
	gdhelpers.c
	gdhelpers.h
	gdkanji.c
	gdtables.c
	gdxpm.c
	jisx0208.h
	wbmp.c
	wbmp.h
)

add_library(${GD_LIB} ${LIBGD_SRC_FILES})

if (WIN32)
	SET_TARGET_PROPERTIES(${GD_LIB} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:msvcrt.lib")
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/  ${GD_SOURCE_DIR}/src)

target_link_libraries(${GD_LIB} ${FREETYPE_LIBRARIES} ${PNG_LIBRARIES} ${JPEG_LIBRARIES} ${XPM_LIBRARIES} ${FONTCONFIG_LIBRARIES})


