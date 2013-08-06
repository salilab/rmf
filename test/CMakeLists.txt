
include("${PROJECT_SOURCE_DIR}/test/PyTests.cmake")
include("${PROJECT_SOURCE_DIR}/test/CppTests.cmake")

foreach (test ${python_tests})
   GET_FILENAME_COMPONENT(name ${test} NAME_WE)
  add_test("RMF.${name}" "python" ${test})
  set_tests_properties("RMF.${name}"
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}")
  set_tests_properties("RMF.${name}" PROPERTIES LABELS "RMF;test")
  set_tests_properties("RMF.${name}" PROPERTIES TIMEOUT 4)
  #add_dependencies(${name} RMFPython)
endforeach()

foreach (test ${cpp_tests})
   GET_FILENAME_COMPONENT(name ${test} NAME_WE)
   add_executable("RMF.${name}" ${test})
   target_link_libraries("RMF.${name}" RMF)
   set_target_properties("RMF.${name}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test")
   set_property(TARGET "RMF.${name}" PROPERTY FOLDER "RMF")
   add_test("RMF.${name}"
            "${CMAKE_BINARY_DIR}/test/RMF.${name}${CMAKE_EXECUTABLE_SUFFIX}")
   set_tests_properties("RMF.${name}" PROPERTIES LABELS "RMF;test")
   set_tests_properties("RMF.${name}" PROPERTIES TIMEOUT 4)
   set(tests ${tests} "RMF.${name}")
endforeach()

set(RMF_TESTS ${tests} CACHE INTERNAL "" FORCE)
