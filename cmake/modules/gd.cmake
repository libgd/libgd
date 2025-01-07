option(BUILD_TEST "Compile examples in the build tree and enable ctest" OFF)

# This expects TESTS_FILES to already be defined to the lists of tests.
# Extra libs can be passed in as the 1st argument.
MACRO(ADD_GD_TESTS)
	GET_FILENAME_COMPONENT(TEST_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
	FOREACH(test_name ${TESTS_FILES})
		SET(test_prog_name "test_${TEST_PREFIX}_${test_name}")
		add_executable(${test_prog_name} "${test_name}.c")
		target_link_libraries (${test_prog_name} gdTest ${ARGV0})
		add_test(NAME ${test_prog_name} COMMAND ${test_prog_name})
		SET_PROPERTY(TEST ${test_prog_name} PROPERTY SKIP_RETURN_CODE 77)
	ENDFOREACH(test_name)
ENDMACRO(ADD_GD_TESTS)
