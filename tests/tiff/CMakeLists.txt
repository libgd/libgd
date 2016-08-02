IF(TIFF_FOUND)
SET(TESTS_FILES
	tiff_im2im
	tiff_invalid_read
	tiff_null
	tiff_dpi
)
ENDIF(TIFF_FOUND)

ADD_GD_TESTS()
