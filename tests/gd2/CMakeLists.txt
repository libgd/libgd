IF(ENABLE_GD_FORMATS)
IF(ZLIB_FOUND)
LIST(APPEND TESTS_FILES
	bug_289
	bug00209
	bug00309
	bug00354
	bug00383
	createimagefromgd2part
	createimagefromgd2partptr
	gd2_empty_file
	php_bug_72339
	gd2_null
	gd2_read_corrupt
	too_few_image_data
	gd2_im2im
)

IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	gd2_read
)
ENDIF(PNG_FOUND)
ENDIF(ZLIB_FOUND)
ENDIF(ENABLE_GD_FORMATS)

ADD_GD_TESTS()
