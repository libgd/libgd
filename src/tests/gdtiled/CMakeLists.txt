add_executable(bug00032 bug00032.c)

target_link_libraries (bug00032 gdTest ${GD_LIB})

ADD_TEST(bug00032 ${EXECUTABLE_OUTPUT_PATH}/bug00032)
