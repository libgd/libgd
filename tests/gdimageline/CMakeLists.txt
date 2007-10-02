SET(TESTS_FILES
	gdimageline_aa
	gdimageline_aa_outofrange
	bug00077
	bug00072
)

FOREACH(test_name ${TESTS_FILES})
	add_executable(${test_name} "${test_name}.c")
	target_link_libraries (${test_name} gdTest ${GD_LIB})
	ADD_TEST(${test_name} ${EXECUTABLE_OUTPUT_PATH}/${test_name})
ENDFOREACH(test_name)
