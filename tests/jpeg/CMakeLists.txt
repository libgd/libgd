IF(JPEG_FOUND)
LIST(APPEND TESTS_FILES
	jpeg_im2im
	jpeg_null
	jpeg_read
	jpeg_empty_file
	jpeg_resolution
	bug00338
	bug_github_18
)
ENDIF(JPEG_FOUND)

ADD_GD_TESTS()
