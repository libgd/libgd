IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	basic
	basic_alpha
	exact_alpha
	bug00201
)
ENDIF(PNG_FOUND)

ADD_GD_TESTS()
