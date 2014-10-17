
SET(TESTS_FILES
	gd2_read
	gd2_empty_file
	gd2_im2im
	gd2_null
)

FOREACH(test_name ${TESTS_FILES})
	add_executable(${test_name} "${test_name}.c")
	target_link_libraries (${test_name} gdTest)
	add_test(NAME ${test_name} COMMAND ${test_name})
ENDFOREACH(test_name)
