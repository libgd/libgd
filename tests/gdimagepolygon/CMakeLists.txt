IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	gdimagepolygon0
	gdimagepolygon1
	gdimagepolygon2
	gdimagepolygon3
)
ENDIF(PNG_FOUND)

ADD_GD_TESTS()
