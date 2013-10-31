include(Files.cmake)

include_directories(${PROJECT_SOURCE_DIR}/src)

foreach (test ${pyfiles})
   GET_FILENAME_COMPONENT(name ${test} NAME_WE)
  add_test("RMF.${name}" "python" ${PROJECT_SOURCE_DIR}/test/${test})
  set_tests_properties("RMF.${name}"
    PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/lib")
  set_tests_properties("RMF.${name}" PROPERTIES LABELS "RMF;test")
  set_tests_properties("RMF.${name}" PROPERTIES TIMEOUT 8)
  #add_dependencies(${name} RMFPython)
endforeach()

foreach (test ${cppfiles})
   GET_FILENAME_COMPONENT(name ${test} NAME_WE)
   add_executable("RMF-${name}" ${test})
   add_dependencies("RMF-${name}" RMF-decorators)
   target_link_libraries("RMF-${name}" RMF-lib)
   set_target_properties("RMF-${name}" PROPERTIES
                          RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/test"
                          OUTPUT_NAME ${name})
   set_property(TARGET "RMF-${name}" PROPERTY FOLDER "RMF")
   add_test("RMF-${name}"
            "${PROJECT_BINARY_DIR}/test/${name}${CMAKE_EXECUTABLE_SUFFIX}")
   set_tests_properties("RMF-${name}" PROPERTIES LABELS "RMF;test")
   set_tests_properties("RMF-${name}" PROPERTIES TIMEOUT 8)
   list(APPEND tests "RMF-${name}")
endforeach()

set(RMF_TESTS ${tests} CACHE INTERNAL "" FORCE)
