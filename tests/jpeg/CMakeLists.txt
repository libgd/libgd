IF(JPEG_FOUND)
LIST(APPEND TESTS_FILES
	jpeg_empty_file
	jpeg_im2im
	jpeg_null
)

IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	bug_github_18
	jpeg_read
	jpeg_resolution
	bug00338
)
ENDIF(PNG_FOUND)
ENDIF(JPEG_FOUND)

ADD_GD_TESTS()
