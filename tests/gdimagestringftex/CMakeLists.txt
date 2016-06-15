IF(FREETYPE_FOUND)
SET(TESTS_FILES
	gdimagestringftex_returnfontpathname
)
ENDIF(FREETYPE_FOUND)

ADD_GD_TESTS()
