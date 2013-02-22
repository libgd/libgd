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
	get_target_property(test_path ${test_name} LOCATION)
	ADD_TEST(${test_name} ${test_path})
ENDFOREACH(test_name)
