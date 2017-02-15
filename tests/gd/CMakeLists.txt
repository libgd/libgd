LIST(APPEND TESTS_FILES
	gd_null
	gd_num_colors
	gd_versiontest
)

IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	gd_im2im
)
ENDIF(PNG_FOUND)

ADD_GD_TESTS()
