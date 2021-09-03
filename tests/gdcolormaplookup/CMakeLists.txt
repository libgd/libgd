if (NOT WIN32)
LIST(APPEND TESTS_FILES
	gdcolormaplookup
)
endif (NOT WIN32)
ADD_GD_TESTS()
