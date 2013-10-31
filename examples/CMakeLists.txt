
include("Files.cmake")

foreach (test ${pyfiles})
   GET_FILENAME_COMPONENT(name ${test} NAME_WE)
  add_test("RMF.${name}" "python" ${PROJECT_SOURCE_DIR}/examples/${test})
  set_tests_properties("RMF.${name}"
    PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/lib")
  set_tests_properties("RMF.${name}" PROPERTIES LABELS "RMF;example")
  #add_dependencies(${name} RMFPython)
endforeach()
