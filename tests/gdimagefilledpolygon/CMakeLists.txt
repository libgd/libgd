IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	gdimagefilledpolygon0
	gdimagefilledpolygon1
	gdimagefilledpolygon2
	gdimagefilledpolygon3
	bug00100
	php_bug_64641
	self_intersecting
)
ENDIF(PNG_FOUND)

ADD_GD_TESTS()
