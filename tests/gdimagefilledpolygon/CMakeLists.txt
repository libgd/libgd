SET(TESTS_FILES
	gdimagefilledpolygon0
	gdimagefilledpolygon1
	gdimagefilledpolygon2
	gdimagefilledpolygon3
	bug00100
)

FOREACH(test_name ${TESTS_FILES})
	add_executable(${test_name} "${test_name}.c")
	target_link_libraries (${test_name} gdTest)
	add_test(NAME ${test_name} COMMAND ${test_name})
ENDFOREACH(test_name)
