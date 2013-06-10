
SET (LIBGD_SRC_FILES
	gd.c
	gdfx.c
	gdfx.h
	gd_color.c
	gd_color.h
	gd_color_map.c
	gd_color_map.h
	gd_crop.c
	gd_transform.c
	gd_security.c
	gdpp.cxx
	gdpp.h
	gd_bmp.c
	bmp.h
	gd_filter.c
	gd_gd.c
	gd_gd2.c
	gd_io.c
	gd_io.h
	gd_io_stream.cxx
	gd_io_stream.h
	gd_io_dp.c
	gd_interpolation.c
	gd_gif_in.c
	gd_gif_out.c
	gd_io_file.c
	gd_io_ss.c
	gd_jpeg.c
	gd_matrix.c
	gd_nnquant.c
	gd_nnquant.h
	gd_png.c
	gd_rotate.c
	gd_tiff.c
	gd_tga.c
	gd_tga.h
	gd_ss.c
	gd_topal.c
	gd_wbmp.c
	gdcache.c
	gdcache.h
	gdfontg.c
	gdfontg.h
	gdfontl.c
	gdfontl.h
	gdfontmb.c
	gdfontmb.h
	gdfonts.c
	gdfonts.h
	gdfontt.c
	gdfontt.h
	gdft.c
	gdhelpers.c
	gdhelpers.h
	gdkanji.c
	gdtables.c
	gdxpm.c
	jisx0208.h
	wbmp.c
	gd.h
	wbmp.h
	gd_webp.c
	webpimg.c
	gd_xbm.c
)

add_library(${GD_LIB} ${LIBGD_SRC_FILES})
add_library(${GD_LIB_STATIC} STATIC  ${LIBGD_SRC_FILES})

if (WIN32 AND NOT MINGW AND NOT MSYS)
#	SET_TARGET_PROPERTIES(${GD_LIB} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:msvcrt.lib")
SET_PROPERTY(TARGET ${GD_LIB_STATIC} APPEND PROPERTY COMPILE_DEFINITIONS NONDLL=1)
ENDIF(WIN32 AND NOT MINGW AND NOT MSYS)

if (MINGW OR MSYS)
	ADD_DEFINITIONS("-mms-bitfields")
endif (MINGW OR MSYS)

INCLUDE_DIRECTORIES(BEFORE "${PROJECT_BINARY_DIR}" "${CMAKE_BINARY_DIR}" "${GD_SOURCE_DIR}/src")

target_link_libraries(${GD_LIB} ${ZLIB_LIBRARIES} ${FREETYPE_LIBRARIES} ${PNG_LIBRARIES} ${LIQ_LIBRARIES} ${JPEG_LIBRARIES} ${TIFF_LIBRARIES} ${XPM_LIBRARIES} ${FONTCONFIG_LIBRARY} ${VPX_LIBRARIES})
target_link_libraries(${GD_LIB_STATIC} ${ZLIB_LIBRARIES} ${FREETYPE_LIBRARIES} ${PNG_LIBRARIES} ${LIQ_LIBRARIES} ${JPEG_LIBRARIES} ${TIFF_LIBRARIES} ${XPM_LIBRARIES} ${FONTCONFIG_LIBRARY} ${VPX_LIBRARIES} )

set(GD_PROGRAMS annotate gdparttopng gdtopng gd2copypal gd2topng pngtogd pngtogd2 webpng gd2togif gdcmpgif giftogd2)
foreach(program ${GD_PROGRAMS})
    add_executable(${program} ${program}.c)
    target_link_libraries(${program} ${GD_LIB})
endforeach(program)

install(TARGETS ${GD_LIB} ${GD_LIB_STATIC} ${GD_PROGRAMS}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(PROGRAMS bdftogd DESTINATION bin)
install(FILES gd.h gdfx.h gd_io.h gdcache.h gdfontg.h gdfontl.h gdfontmb.h gdfonts.h gdfontt.h gd_color_map.h gd_errors.h DESTINATION include)
