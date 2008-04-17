
include_directories (BEFORE ${GD_SOURCE_DIR}/src "${CMAKE_BINARY_DIR}")


SET(TESTS_FILES
  tiffread
#  resize
  copyrotated
#  ellipseaa
#  ellipse
  arc
#  ellipsearc
  flip
  crop
#  ellfullaa
  tgaread
  nnquant
  gif
)

if (JPEG_FOUND)
	SET(TESTS_FILES
		"${TESTS_FILES}"
		nnquant
	)
endif (JPEG_FOUND)


if (TIFF_FOUND)
	SET(TESTS_FILES
		"${TESTS_FILES}"
		tiffread
	)
endif (TIFF_FOUND)

FOREACH(test_name ${TESTS_FILES})
	add_executable(${test_name} "${test_name}.c")
	target_link_libraries (${test_name} ${GD_LIB})
ENDFOREACH(test_name)

if (WIN32)
	add_executable(windows WIN32 "windows.c")
	target_link_libraries (${test_name} ${GD_LIB})
endif (WIN32)
