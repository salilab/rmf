imp_get_process_exit_code(ElementTree has_element_tree ${CMAKE_BINARY_DIR} COMMAND python -c "import xml.etree.ElementTree")

if(${has_element_tree})
message(STATUS "Python xml.etree not found, docs disabled.")
    set(IMP_DOXYGEN_FOUND False CACHE INTERNAL "")
    set(IMP_DOXYGEN_EXECUTABLE "${DOXYGEN_EXECUTABLE}" CACHE INTERNAL "")
else()
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
  imp_get_process_output("Doxygen version" doxygen_version ${CMAKE_BINARY_DIR} COMMAND ${DOXYGEN_EXECUTABLE} --version)
  message(STATUS "Doxygen is ${DOXYGEN_EXECUTABLE}, version ${doxygen_version}")
  set(IMP_DOXYGEN_FOUND True CACHE INTERNAL "")
  set(IMP_DOXYGEN_EXECUTABLE ${DOXYGEN_EXECUTABLE} CACHE INTERNAL "")
else()
  message(STATUS "Doxygen not found")
endif(DOXYGEN_FOUND)

endif(${has_element_tree})
