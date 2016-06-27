IF(JPEG_FOUND)
SET(TESTS_FILES
	jpeg_im2im
	jpeg_null
	jpeg_read
	jpeg_empty_file
	jpeg_resolution
	bug_github_18
)
ENDIF(JPEG_FOUND)

ADD_GD_TESTS()
