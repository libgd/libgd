include_directories (BEFORE "${GD_SOURCE_DIR}/src" "${CMAKE_BINARY_DIR}")

SET(TESTS_FILES
	gif
	uhdr
	gif_rewrite
)

if (PNG_FOUND)
	LIST(APPEND TESTS_FILES arc crop gif_frames tgaread)

	if (JPEG_FOUND)
		LIST(APPEND TESTS_FILES copyrotated flip nnquant imagescale)
	endif (JPEG_FOUND)
endif (PNG_FOUND)

if (JPEG_FOUND)
	LIST(APPEND TESTS_FILES resize)
endif (JPEG_FOUND)

if (TIFF_FOUND)
	LIST(APPEND TESTS_FILES	tiffread tiff_write_pages)
	if (PNG_FOUND)
		LIST(APPEND TESTS_FILES tiff_pages)
	endif (PNG_FOUND)
endif (TIFF_FOUND)

if (HEIF_FOUND)
	LIST(APPEND TESTS_FILES	png2heif)
endif (HEIF_FOUND)

if (WEBP_FOUND)
	LIST(APPEND TESTS_FILES webp_rewrite)
	if (PNG_FOUND)
		LIST(APPEND TESTS_FILES webp_frames)
	endif (PNG_FOUND)
endif (WEBP_FOUND)

if (BUILD_SHARED_LIBS)
	set(GD_LINK_LIB ${GD_LIB})
else()
	set(GD_LINK_LIB ${GD_LIB_STATIC})
endif()

FOREACH(test_name ${TESTS_FILES})
	add_executable(${test_name} "${test_name}.c")
	target_link_libraries (${test_name} ${GD_LINK_LIB})
ENDFOREACH(test_name)

# Add libraries *after* we've called add_executable on all the tests.

if (WIN32 AND NOT MINGW AND NOT MSYS)
	add_executable(windows WIN32 "windows.c")
	target_link_libraries (windows ${GD_LINK_LIB})
else (WIN32 AND NOT MINGW AND NOT MSYS)
	if (MINGW OR MSYS)
		add_executable(windows "windows.c")
		add_definitions("-mwindows")
		target_link_libraries(windows ${GD_LINK_LIB} stdc++ gdi32)
	endif (MINGW OR MSYS)
endif (WIN32 AND NOT MINGW AND NOT MSYS)
