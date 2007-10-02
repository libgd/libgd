add_executable(bug00007 bug00007.c)
target_link_libraries (bug00007 gdTest ${GD_LIB})
ADD_TEST(bug00007 ${EXECUTABLE_OUTPUT_PATH}/bug00007)
