
file(GLOB python_tests "test_*.py")

foreach (test ${python_tests})
   GET_FILENAME_COMPONENT(name ${test} NAME_WE)
  add_test(${name} "python" ${test})
  set_tests_properties(${name}
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}")
  #add_dependencies(${name} RMFPython)
endforeach()

file(GLOB cpp_tests "test_*.cpp")

foreach (test ${cpp_tests})
   GET_FILENAME_COMPONENT(name ${test} NAME_WE)
   add_executable(${name} ${test})
   target_link_libraries(${name} RMF)
   add_test(${name} ${name})
endforeach()
