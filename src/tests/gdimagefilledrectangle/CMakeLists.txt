
add_executable(bug00004 bug00004.c)
target_link_libraries (bug00004 gdTest ${GD_LIB})
ADD_TEST(bug00004 ${EXECUTABLE_OUTPUT_PATH}/bug00004)
