IF(AVIF_FOUND)
LIST(APPEND TESTS_FILES
	avif_ptr_double_free
	avif_im2im
	avif_null
	bad_input
)

IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	compare_avif_to_png
)
ENDIF(PNG_FOUND)

ENDIF(AVIF_FOUND)

ADD_GD_TESTS()
