IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	gdimagecopyresized
	github_bug_00638_transparent_png
)
ENDIF(PNG_FOUND)

ADD_GD_TESTS()
