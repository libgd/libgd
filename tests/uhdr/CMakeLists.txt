IF(HAVE_LIBUHDR)
LIST(APPEND TESTS_FILES
	uhdr_null
	uhdr_plain_jpeg_rejected
	uhdr_roundtrip
	uhdr_ptr_roundtrip
)

ADD_GD_TESTS()
ENDIF(HAVE_LIBUHDR)
