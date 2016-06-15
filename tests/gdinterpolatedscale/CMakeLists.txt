LIST(APPEND TESTS_FILES
	gdModesAndPalettes
	gdTrivialResize
)

ADD_GD_TESTS()

# Probably should speed this test up.
set_tests_properties(test_${TEST_PREFIX}_gdTrivialResize PROPERTIES TIMEOUT 20)
