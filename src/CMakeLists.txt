SET (LIBGD_SRC_FILES
	bmp.h
	gd.c
	gd.h
	gd_bmp.c
	gd_color.c
	gd_color.h
	gd_color_map.c
	gd_color_map.h
	gd_color_match.c
	gd_crop.c
	gd_filename.c
	gd_filter.c
	gd_gd.c
	gd_gd2.c
	gd_gif_in.c
	gd_gif_out.c
	gd_intern.h
	gd_interpolation.c
	gd_io.c
	gd_io.h
	gd_io_dp.c
	gd_io_file.c
	gd_io_ss.c
	gd_io_stream.cxx
	gd_io_stream.h
	gd_jpeg.c
	gd_matrix.c
	gd_nnquant.c
	gd_nnquant.h
	gd_png.c
	gd_rotate.c
	gd_security.c
	gd_ss.c
	gd_tga.c
	gd_tga.h
	gd_tiff.c
	gd_topal.c
	gd_transform.c
	gd_version.c
	gd_wbmp.c
	gd_webp.c
	gd_xbm.c
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
	gdfx.c
	gdfx.h
	gdhelpers.c
	gdhelpers.h
	gdkanji.c
	gdpp.cxx
	gdpp.h
	gdtables.c
	gdxpm.c
	jisx0208.h
	wbmp.c
	wbmp.h
)

# Static library just for utility programs.
SET (GD_PROGRAMS_LIB_SRC_FILES)

if(MSVC)
	set(GD_PROGRAMS_LIB_SRC_FILES ${GD_PROGRAMS_LIB_SRC_FILES}
		getopt.c
	)
	if(MSVC_VERSION LESS 1900)
		set(LIBGD_SRC_FILES ${LIBGD_SRC_FILES}
			snprintf.c
		)
	endif(MSVC_VERSION LESS 1900)
endif(MSVC)

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

if (NOT "${GD_PROGRAMS_LIB_SRC_FILES}" STREQUAL "")
	add_library(gd_programs_lib STATIC ${GD_PROGRAMS_LIB_SRC_FILES})
endif()

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
	${RAQM_LIBRARIES}
)
if (BUILD_SHARED_LIBS)
	target_link_libraries(${GD_LIB} ${LIBGD_DEP_LIBS})
endif()
if (BUILD_STATIC_LIBS)
	target_link_libraries(${GD_LIB_STATIC} ${LIBGD_DEP_LIBS})
endif()

set(GD_PROGRAMS gdcmpgif)

if (PNG_FOUND)
	set(GD_PROGRAMS ${GD_PROGRAMS} gdtopng pngtogd webpng)
	if (ZLIB_FOUND)
		set(GD_PROGRAMS ${GD_PROGRAMS} gdparttopng gd2topng pngtogd2)
	endif()
endif()

if (FREETYPE_FOUND AND JPEG_FOUND)
	set(GD_PROGRAMS ${GD_PROGRAMS} annotate)
endif()

if (ZLIB_FOUND)
	set(GD_PROGRAMS ${GD_PROGRAMS} gd2copypal gd2togif giftogd2)
endif()

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
install(PROGRAMS bdftogd DESTINATION bin)
install(FILES
	entities.h
	gd.h
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
