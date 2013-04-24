
SET(TESTS_FILES
	bmp_im2im
	bmp_null
)

FOREACH(test_name ${TESTS_FILES})
	add_executable(${test_name} "${test_name}.c")
	target_link_libraries (${test_name} gdTest ${GD_LIB})
	get_target_property(test_path ${test_name} LOCATION)
	ADD_TEST(${test_name} ${test_path})
ENDFOREACH(test_name)
