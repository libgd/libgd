if(FONTCONFIG_FOUND)
IF(FREETYPE_FOUND)
	# Skip fontconfig tests when no fonts are installed (e.g. minimal containers)
	execute_process(
		COMMAND fc-list
		OUTPUT_VARIABLE FC_LIST_OUTPUT
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	if(NOT FC_LIST_OUTPUT STREQUAL "")
		LIST(APPEND TESTS_FILES
			basic
		)
	else()
		message(STATUS "Fontconfig has no fonts available - skipping fontconfig/basic test")
	endif()
ENDIF(FREETYPE_FOUND)
ENDIF(FONTCONFIG_FOUND)

ADD_GD_TESTS()
