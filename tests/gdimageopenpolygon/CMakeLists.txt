IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	gdimageopenpolygon0
	gdimageopenpolygon1
	gdimageopenpolygon2
	gdimageopenpolygon3
)
ENDIF(PNG_FOUND)

ADD_GD_TESTS()
