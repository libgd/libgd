
add_executable(bug00003 bug00003.c)
target_link_libraries (bug00003 gdTest ${GD_LIB})
ADD_TEST(bug00003 ${EXECUTABLE_OUTPUT_PATH}/bug00003)
