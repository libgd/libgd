SET(GDTEST_TOP_DIR "${GD_SOURCE_DIR}/tests")
CONFIGURE_FILE(test_config.h.cmake test_config.h ESCAPE_QUOTES)
add_library (gdTest STATIC gdtest.c)
target_link_libraries(gdTest ${GD_LIB})
