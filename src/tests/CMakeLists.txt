
if (BUILD_TEST)
	SET(DART_TESTING_TIMEOUT 5)

	INCLUDE(Dart)
	#ENABLE_TESTING()
	#set(GD_SOURCE_DIR "/home/pierre/cvs/gd/libgd")
	#set(GD_INCLUDE_DIR "/home/pierre/cvs/gd/libgd")
	#set(GD_LIBS_DIR "/home/pierre/cvs/gd/libgd/.libs")

	set(GDTEST_SOURCE_DIR ${GD_SOURCE_DIR}/tests/gdtest)

	# If build in the src tree or set the libs in the lines before
	#SET(GD_LIB gd)

	SET(CMAKE_BUILD_TYPE TESTINGGCOV)
	SET(CMAKE_CXX_FLAGS_TESTINGGCOV "-g3 -O0")
	SET(CMAKE_C_FLAGS_TESTINGGCOV "-g3 -O0")

	message(STATUS "gd include dir: ${GD_INCLUDE_DIR}" )
	message(STATUS "gd libs dir: ${GD_LIBS_DIR}" )

	include_directories (BEFORE ${GD_INCLUDE_DIR} "${GDTEST_SOURCE_DIR}")
	link_directories ("${GD_LIBS_DIR}" "${GDTEST_BINARY_DIR}/gdtest")

	#FIND_LIBRARY(GD_LIB "gd" PATHS ${GD_LIBS_DIR})
	#message(STATUS "GD_LIB: ${GD_LIB}")

	SET(TESTS_DIRS
		gdtest
		gdimagecolorclosest
		gdimagecolorexact
		gdimagecolorresolve
		#gdimageellipse
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
	)

	FOREACH(test_dir ${TESTS_DIRS})
		add_subdirectory (${test_dir})
	ENDFOREACH(test_dir)
endif (BUILD_TEST)