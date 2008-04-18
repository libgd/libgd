add_executable(bug00032 bug00032.c)

target_link_libraries (bug00032 ${GDTESTS_TARGET_LINK})

ADD_TEST(bug00032 ${EXECUTABLE_OUTPUT_PATH}/bug00032)
