
SET(TESTS_FILES
	bug00005
	bug00005_2
	bug00006
	bug00060
	bug00066
	bug00181
)

FOREACH(test_name ${TESTS_FILES})
	add_executable(${test_name} "${test_name}.c")
	target_link_libraries (${test_name} ${GDTESTS_TARGET_LINK})
	ADD_TEST(${test_name} ${test_name})
ENDFOREACH(test_name)
