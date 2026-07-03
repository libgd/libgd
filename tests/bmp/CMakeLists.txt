LIST(APPEND TESTS_FILES
	bmp_null
	bug00275
	bug00450
	bmp_im2im
	bug00276
	bmp_size
	bmp_write_ex
)

IF(PNG_FOUND)
	LIST(APPEND TESTS_FILES bmpsuite_read)
ENDIF(PNG_FOUND)

ADD_GD_TESTS()
