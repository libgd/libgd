
message("inc: ${GD_INCLUDE_DIR}")
include_directories (BEFORE ${GD_SOURCE_DIR}/src "${CMAKE_BINARY_DIR}")


SET(TESTS_FILES
  tiffread	
)

FOREACH(test_name ${TESTS_FILES})
	add_executable(${test_name} "${test_name}.c")
	target_link_libraries (${test_name} ${GD_LIB})
	ADD_TEST(${test_name} ${EXECUTABLE_OUTPUT_PATH}/${test_name})
ENDFOREACH(test_name)
