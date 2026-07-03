IF(HAVE_LIBUHDR)
LIST(APPEND TESTS_FILES
	uhdr_null
	uhdr_plain_jpeg_rejected
	uhdr_roundtrip
	uhdr_ptr_roundtrip
)

IF(NOT WIN32)
LIST(APPEND TESTS_FILES
	uhdr_cli_parity_noop
	uhdr_cli_parity_quality_noop
	uhdr_metadata_resize
)
ENDIF(NOT WIN32)

ADD_GD_TESTS()
ENDIF(HAVE_LIBUHDR)
