add_definitions(-DGDTEST_TOP_DIR="${CMAKE_CURRENT_SOURCE_DIR}/..")
add_library (gdTest STATIC gdtest.c)
target_link_libraries(gdTest ${GD_LIB})
