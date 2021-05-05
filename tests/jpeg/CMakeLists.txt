IF(JPEG_FOUND)
LIST(APPEND TESTS_FILES
	jpeg_empty_file
	jpeg_im2im
	jpeg_ptr_double_free
	jpeg_null
	jpeg_resolution
)

IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	bug_github_18
	jpeg_read
	bug00338
)
ENDIF(PNG_FOUND)
ENDIF(JPEG_FOUND)

ADD_GD_TESTS()
