IF(TIFF_FOUND)
LIST(APPEND TESTS_FILES
	tiff_im2im
	tiff_invalid_read
	tiff_null
	tiff_dpi
)

IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	tiff_read_bw
)
ENDIF(PNG_FOUND)
ENDIF(TIFF_FOUND)

ADD_GD_TESTS()
