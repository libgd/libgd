
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

	include_directories (BEFORE ${GD_INCLUDE_DIR} "${GDTEST_SOURCE_DIR}" "${CMAKE_BINARY_DIR}")

	SET(TESTS_DIRS
		gdtest
		gdimagearc
		gdimagecolorclosest
		gdimagecolorexact
		gdimagecolorreplace
		gdimagecolorresolve
		gdimagecolordeallocate
		gdimagecolortransparent
		#gdimageellipse
		gdimagepixelate
		gdimageline
		gdimagefilledellipse
		gdimagecopyrotated
		gdtiled
		gdimagerectangle
		gdimagefilledrectangle
		gd2
		gif
		jpeg
		png
		gdimagecopy
		gdimagefill
		gdimagefilltoborder
		freetype
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
