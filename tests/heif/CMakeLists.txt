IF(HEIF_FOUND)
LIST(APPEND TESTS_FILES
	avif_ptr_double_free
	heif_im2im
	heif_null
	heif_ptr_double_free
	heif_read
)
ENDIF(HEIF_FOUND)

ADD_GD_TESTS()
