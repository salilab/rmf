# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${PROJECT_SOURCE_DIR}/doc/Doxyfile.in ${PROJECT_BINARY_DIR}/doc/Doxyfile @ONLY)
add_custom_target(RMF-doc
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/examples/simple.pdb ${CMAKE_BINARY_DIR}/doc/html/simple.pdb.txt
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/examples/3U7W.pdb ${CMAKE_BINARY_DIR}/doc/html/3U7W.pdb.txt
        COMMAND ${CMAKE_BINARY_DIR}/bin/rmf_show -v ${CMAKE_SOURCE_DIR}/examples/3U7W.rmf3 > ${CMAKE_BINARY_DIR}/doc/html/3U7W.txt
        COMMAND ${CMAKE_BINARY_DIR}/bin/rmf_show -v ${CMAKE_SOURCE_DIR}/examples/simple.rmf3 > ${CMAKE_BINARY_DIR}/doc/html/simple.txt
        COMMAND ${CMAKE_BINARY_DIR}/bin/rmf3_dump ${CMAKE_SOURCE_DIR}/examples/3U7W.rmf3 > ${CMAKE_BINARY_DIR}/doc/html/3U7W.rmf3.txt
        COMMAND ${CMAKE_BINARY_DIR}/bin/rmf3_dump ${CMAKE_SOURCE_DIR}/examples/simple.rmf3 > ${CMAKE_BINARY_DIR}/doc/html/simple.rmf3.txt
        COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/doc/Doxyfile
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/doc
        DEPENDS RMF-rmf_show
        COMMENT "Generating API documentation with Doxygen" VERBATIM
        )
  set_property(TARGET "RMF-doc" PROPERTY FOLDER "RMF")
endif(DOXYGEN_FOUND)
