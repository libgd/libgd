
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
	add_test(NAME ${test_name} COMMAND ${test_name})
ENDFOREACH(test_name)
