IF(JXL_FOUND)
ADD_DEFINITIONS(-DHAVE_LIBJXL)
LIST(APPEND TESTS_FILES
	jxl_null
	jxl_im2im
	jxl_lossless_im2im
	jxl_alpha
	jxl_read_file
	jxl_palette
	jxl_anim
	jxl_anim_lossless
	jxl_anim_raw
	jxl_still_as_anim
)
ENDIF(JXL_FOUND)

ADD_GD_TESTS()
