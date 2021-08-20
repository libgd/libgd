MESSAGE("Looking for naturaldocs....")

find_package(Perl 5.8 COMPONENTS Runtime)
find_package(NaturalDocs 1.51)

if(PERL_FOUND)
  SET(NATURALDOCS_FLAGS
  --rebuild --rebuild-output --documented-only
  -i "${CMAKE_CURRENT_BINARY_DIR}/tmp/" 
  -img "${CMAKE_CURRENT_SOURCE_DIR}/naturaldocs/images/"
  -o html "${CMAKE_CURRENT_BINARY_DIR}/api"
  --project "${CMAKE_CURRENT_SOURCE_DIR}/naturaldocs/project/"
  -s Default libgd
  )

  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs/api")
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tmp")
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/api")

  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/../COPYING  ${CMAKE_CURRENT_BINARY_DIR}/tmp/License.txt COPYONLY)
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/../docs/INSTALL  ${CMAKE_CURRENT_BINARY_DIR}/tmp/INSTALL.txt COPYONLY)
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/../docs/INSTALL.Solaris  ${CMAKE_CURRENT_BINARY_DIR}/tmp/INSTALL_SOLARIS.txt COPYONLY)
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/../docs/README.CMAKE  ${CMAKE_CURRENT_BINARY_DIR}/tmp/INSTALL_CMAKE.txt COPYONLY)
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/../docs/README.TESTING  ${CMAKE_CURRENT_BINARY_DIR}/tmp/TESTING.txt COPYONLY)
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/../docs/README.TXT  ${CMAKE_CURRENT_BINARY_DIR}/tmp/README.txt COPYONLY)
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/../docs/README.JPN  ${CMAKE_CURRENT_BINARY_DIR}/tmp/README_JPN.txt COPYONLY)

  execute_process(
    COMMAND sed -e s/@VERSION@/${GDLIB_VERSION}/g ${CMAKE_CURRENT_SOURCE_DIR}/naturaldocs/preamble.txt
    OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/tmp/preamble.txt
    RESULT_VARIABLE TEST_ERROR
  )
  
  SET(PERL_ARGS ${CMAKE_CURRENT_SOURCE_DIR}/../src ${CMAKE_CURRENT_BINARY_DIR}/tmp)
  MESSAGE("${PERL_EXECUTABLE}" ${CMAKE_CURRENT_SOURCE_DIR}/naturaldocs/nobgd.pl ${PERL_ARGS})
  execute_process(COMMAND "${PERL_EXECUTABLE}" ${CMAKE_CURRENT_SOURCE_DIR}/naturaldocs/nobgd.pl ${PERL_ARGS}
    RESULT_VARIABLE TEST_ERROR
    )
    MESSAGE(${TEST_ERROR})
  if(NOT DEBUG_MODE)
    list(APPEND NATURALDOCS_FLAGS --quiet)
  endif()

  add_custom_target(docs
    ${NATURALDOCS_EXECUTABLE} ${NATURALDOCS_FLAGS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation"
    )

  #file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/NaturalDocs-config")
else()
  message(STATUS "SKIPPING generation of API documentation for JavaScript (Perl not found)")
endif()
