
SET(TESTS_FILES
	gif_im2im
	gif_null
	bug00005
	bug00005_2
	bug00006
	bug00060
	bug00066
	bug00181
	bug00227
)

FOREACH(test_name ${TESTS_FILES})
	add_executable(${test_name} "${test_name}.c")
	target_link_libraries (${test_name} gdTest)
	get_target_property(test_path ${test_name} LOCATION)
	ADD_TEST(${test_name} ${test_path})
ENDFOREACH(test_name)
