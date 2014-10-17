
SET(TESTS_FILES
	bug00002_1
	bug00002_2
	bug00002_3
	bug00002_4
)
FOREACH(test_name ${TESTS_FILES})
	add_executable(${test_name} "${test_name}.c")
	target_link_libraries (${test_name} gdTest)
	add_test(NAME ${test_name} COMMAND ${test_name})
ENDFOREACH(test_name)
