LIST(APPEND TESTS_FILES
	github_bug_501
	github_bug_170
	xbm_invalid_dimensions
)

IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	github_bug_109
	x10_basic_read
)
ENDIF(PNG_FOUND)

ADD_GD_TESTS()
