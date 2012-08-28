cmake_minimum_required (VERSION 2.6)
project (RMF)

#INCLUDE (CheckIncludeFiles)
#CHECK_INCLUDE_FILES(google/protobuf/message.h RMF_USE_PROTOBUF)


# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/include/RMF/config.h.in"
  "${PROJECT_BINARY_DIR}/include/RMF/config.h"
  )
set (RMF_VERSION_MAJOR 1)
set (RMF_VERSION_MINOR 0)


include_directories ("${PROJECT_SOURCE_DIR}/include")
include_directories ("${PROJECT_BINARY_DIR}/include")

find_package( Boost REQUIRED COMPONENTS thread system program_options )
#FIND_PROGRAM(RMF_USE_PROTOBUF protoc)




add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(pyext)
add_subdirectory(bin)

# tests
#FILE(GLOB python_tests test/test_*.py)
#foreach (t ${python_tests})
 # add_test(${t} ${t})
 # set_tests_properties(${t} RETURN 0)
#endforeach(t)
