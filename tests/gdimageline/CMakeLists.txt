SET(TESTS_FILES
	gdimageline_aa
	gdimageline_aa_outofrange
	bug00077
	bug00072
	bug00111
)

FOREACH(test_name ${TESTS_FILES})
	add_executable(${test_name} "${test_name}.c")
	target_link_libraries (${test_name} gdTest)
	add_test(NAME ${test_name} COMMAND ${test_name})
ENDFOREACH(test_name)
