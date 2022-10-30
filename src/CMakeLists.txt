SET (LIBGD_SRC_FILES
	gd.c
	gd_avif.c
	gd_avif_metadata.c
	gd_bmp.c
	gd_color.c
	gd_color_map.c
	gd_color_match.c
	gd_perceptual_diff.c
	gd_compositor.c
	gd_crop.c
	gd_draw.c
	gd_draw_blend.c
	gd_filename.c
	gd_readimage.c
	gd_filter.c
	gd_gradient.c
	gd_gd.c
	gd_gd2.c
	gd_gif_in.c
	gd_gif_out.c
	gd_heif.c
	gd_heif_metadata.c
	gd_interesting.c
	gd_interpolation.c
	gd_jxl.c
	gd_io.c
	gd_io_dp.c
	gd_io_file.c
	gd_io_ss.c
	gd_jpeg.c
	gd_matrix.c
	gd_metadata.c
	gd_nnquant.c
	gd_path.c
	gd_path_arc.c
	gd_path_dash.c
	gd_path_matrix.c
	gd_path_stroke.c
	gd_png.c
	gd_qoi.c
	gd_rotate.c
	gd_security.c
	gd_ss.c
	gd_span_rle.c
	gd_surface.c
	gd_text.c
	gd_tga.c
	gd_tiff.c
	gd_topal.c
	gd_transform.c
	gd_uhdr.c
	gd_version.c
	gd_wbmp.c
	gd_webp.c
	gd_xbm.c
	gdcache.c
	gdfontg.c
	gdfontl.c
	gdfontmb.c
	gdfonts.c
	gdfontt.c
	gdft.c
	gdfx.c
	gdhelpers.c
	gdkanji.c
	gdtables.c
	gdxpm.c
	wbmp.c
	gd_array.c
	ftraster/gd_ft_math.c
	ftraster/gd_ft_raster.c
	ftraster/gd_ft_stroker.c
)

if(ENABLE_CPP)
	LIST(APPEND LIBGD_SRC_FILES
		gd_io_stream.cxx
		gdpp.cxx
	)
endif(ENABLE_CPP)

# Static library just for utility programs.
if (BUILD_PROGRAMS)
SET (GD_PROGRAMS_LIB_SRC_FILES)

if(MSVC)
	set(GD_PROGRAMS_LIB_SRC_FILES ${GD_PROGRAMS_LIB_SRC_FILES}
		getopt.c
	)
endif(MSVC)
endif(BUILD_PROGRAMS)

include(GNUInstallDirs)

if (BUILD_SHARED_LIBS)
	add_library(${GD_LIB} ${LIBGD_SRC_FILES})
	set_target_properties(${GD_LIB} PROPERTIES
		SOVERSION ${GDLIB_LIB_SOVERSION}
		VERSION ${GDLIB_LIB_VERSION}
		C_VISIBILITY_PRESET hidden
		CXX_VISIBILITY_PRESET hidden
	)
endif()

if (BUILD_STATIC_LIBS)
	add_library(${GD_LIB_STATIC} STATIC ${LIBGD_SRC_FILES})
	if (UNIX)
		set_target_properties(${GD_LIB_STATIC} PROPERTIES OUTPUT_NAME ${GD_LIB})
	endif()
endif()

if (BUILD_PROGRAMS)
if (NOT "${GD_PROGRAMS_LIB_SRC_FILES}" STREQUAL "")
	add_library(gd_programs_lib STATIC ${GD_PROGRAMS_LIB_SRC_FILES})
endif()
endif(BUILD_PROGRAMS)

if (WIN32 AND NOT MINGW AND NOT MSYS)
  #	SET_TARGET_PROPERTIES(${GD_LIB} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:msvcrt.lib")

  if (BUILD_STATIC_LIBS)
    SET_PROPERTY(TARGET ${GD_LIB_STATIC} APPEND PROPERTY COMPILE_DEFINITIONS NONDLL=1)
  endif()

ENDIF(WIN32 AND NOT MINGW AND NOT MSYS)

if (MINGW OR MSYS)
	ADD_DEFINITIONS("-mms-bitfields")
	if (BUILD_STATIC_LIBS)
		set_target_properties(${GD_LIB_STATIC} PROPERTIES OUTPUT_NAME ${GD_LIB})
	endif()
endif (MINGW OR MSYS)

INCLUDE_DIRECTORIES(BEFORE "${PROJECT_BINARY_DIR}" "${CMAKE_BINARY_DIR}" "${GD_SOURCE_DIR}/src")

SET(LIBGD_DEP_LIBS
	${ZLIB_LIBRARIES}
	${FREETYPE_LIBRARIES}
	${PNG_LIBRARIES}
	${ICONV_LIBRARIES}
	${LIQ_LIBRARIES}
	${JPEG_LIBRARIES}
	${TIFF_LIBRARIES}
	${XPM_LIBRARIES}
	${FONTCONFIG_LIBRARY}
	${WEBP_LIBRARIES}
	${AVIF_LIBRARIES}
	${UHDR_LIBRARIES}
	${RAQM_LIBRARIES}
	${HEIF_LIBRARIES}
	${JXL_LIBRARIES}
)

if (NEED_LINKING_AGAINST_LIBM)
     LIST(APPEND LIBGD_DEP_LIBS m)
endif()

if (BUILD_SHARED_LIBS)
	target_link_libraries(${GD_LIB} ${LIBGD_DEP_LIBS})
endif()
if (BUILD_STATIC_LIBS)
	target_link_libraries(${GD_LIB_STATIC} ${LIBGD_DEP_LIBS})
endif()

SET(LIBS_PRIVATES
	${ICONV_LIBRARIES}
	${LIQ_LIBRARIES}
)

set(GD_PROGRAMS gdcmpgif)

if (PNG_FOUND)
	set(GD_PROGRAMS ${GD_PROGRAMS} gdtopng pngtogd)
	if (NOT MSVC)
		set(GD_PROGRAMS ${GD_PROGRAMS} webpng)
	endif (NOT MSVC)
	if (ZLIB_FOUND)
		set(GD_PROGRAMS ${GD_PROGRAMS} gdparttopng gd2topng pngtogd2)
	endif()
endif()

if (FREETYPE_FOUND AND JPEG_FOUND)
	if (NOT WIN32)
		set(GD_PROGRAMS ${GD_PROGRAMS} annotate)
	endif (NOT WIN32)
endif()

if (ZLIB_FOUND)
	set(GD_PROGRAMS ${GD_PROGRAMS} gd2copypal gd2togif giftogd2)
endif()

if (BUILD_PROGRAMS)
foreach(program ${GD_PROGRAMS})
    add_executable(${program} ${program}.c)

    if (BUILD_SHARED_LIBS)
        target_link_libraries(${program} ${GD_LIB})
    else()
        target_link_libraries(${program} ${GD_LIB_STATIC})
    endif()
	if (NOT "${GD_PROGRAMS_LIB_SRC_FILES}" STREQUAL "")
		target_link_libraries(${program} gd_programs_lib)
	endif()
endforeach(program)
	set(GD_INSTALL_TARGETS ${GD_PROGRAMS})
	install(PROGRAMS bdftogd DESTINATION bin)
endif(BUILD_PROGRAMS)

if (BUILD_SHARED_LIBS)
	set(GD_INSTALL_TARGETS ${GD_INSTALL_TARGETS} ${GD_LIB})
endif()
if (BUILD_STATIC_LIBS)
	set(GD_INSTALL_TARGETS ${GD_INSTALL_TARGETS} ${GD_LIB_STATIC})
endif()

install(TARGETS ${GD_INSTALL_TARGETS}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES
	gd.h
	gd_vector2d.h
	gd_color_map.h
	gd_errors.h
	gd_io.h
	gdcache.h
	gdfontg.h
	gdfontl.h
	gdfontmb.h
	gdfonts.h
	gdfontt.h
	gdfx.h
	gdpp.h
	DESTINATION include)

CONFIGURE_FILE(../config/gdlib.pc.cmake gdlib.pc @ONLY)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/gdlib.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
