
if (BUILD_TEST)
	SET(DART_TESTING_TIMEOUT 5)

	INCLUDE(Dart)

	set(GDTEST_SOURCE_DIR ${GD_SOURCE_DIR}/tests/gdtest)

	IF (ENABLE_GCOV AND CMAKE_COMPILER_IS_GNUCC)
		SET(CMAKE_BUILD_TYPE TESTINGGCOV)
		SET(CMAKE_CXX_FLAGS_TESTINGGCOV "-g3 -O0")
		SET(CMAKE_C_FLAGS_TESTINGGCOV "-g3 -O0")
	endif (ENABLE_GCOV AND CMAKE_COMPILER_IS_GNUCC)

	message(STATUS "gd include dir: ${GD_INCLUDE_DIR}" )
	message(STATUS "gd libs dir: ${GD_LIBS_DIR}" )

	include_directories (BEFORE ${GD_INCLUDE_DIR} "${GDTEST_SOURCE_DIR}")
	link_directories ("${GD_LIBS_DIR}" "${GDTEST_BINARY_DIR}/gdtest")

	include_directories (BEFORE ${GD_INCLUDE_DIR} "${GDTEST_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_BINARY_DIR}/tests/gdtest")

	SET(TESTS_DIRS
		gdtest
		bmp
		freetype
		gd
		gd2
		gdimagearc
		gdimagecolorclosest
		gdimagecolordeallocate
		gdimagecolorexact
		gdimagecolorreplace
		gdimagecolorresolve
		gdimagecolortransparent
		gdimagecopy
		gdimagecopyrotated
		#gdimageellipse
		gdimagefill
		gdimagefilledellipse
		gdimagefilledpolygon
		gdimagefilledrectangle
		gdimagefilltoborder
		gdimageline
		gdimageopenpolygon
		gdimagepixelate
		gdimagepolygon
		gdimagerectangle
		gdimagescatterex
		gdimagesetpixel
		gdimagestringft
		gdimagestringftex
		gdnewfilectx
		gdtiled
		gif
		jpeg
		png
		tga
		tiff
		wbmp
		xpm
	)

	IF (WIN32)
		SET(GDTESTS_TARGET_LINK "gdTest")
	ELSE (WIN32)
		SET(GDTESTS_TARGET_LINK "gdTest")
	ENDIF (WIN32)

	FOREACH(test_dir ${TESTS_DIRS})
		add_subdirectory (${test_dir})
	ENDFOREACH(test_dir)

endif (BUILD_TEST)
