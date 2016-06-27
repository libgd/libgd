IF(PNG_FOUND)
SET(TESTS_FILES
	png_im2im
	png_null
	png_resolution
	bug00011
	bug00033
	bug00086
	bug00088
	bug00193
)
ENDIF(PNG_FOUND)

ADD_GD_TESTS()
