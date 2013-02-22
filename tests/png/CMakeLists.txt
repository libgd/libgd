
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
	get_target_property(test_path ${test_name} LOCATION)
	ADD_TEST(${test_name} ${test_path})
ENDFOREACH(test_name)
