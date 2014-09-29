
SET(TESTS_FILES
	png_im2im
	png_null
	png_resolution
	bug00011
	bug00033
	bug00086
	bug00088
	bug00193
)

FOREACH(test_name ${TESTS_FILES})
	add_executable(${test_name} "${test_name}.c")
	target_link_libraries (${test_name} gdTest)
	add_test(NAME ${test_name} COMMAND ${test_name})
ENDFOREACH(test_name)
