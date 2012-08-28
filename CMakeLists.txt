cmake_minimum_required (VERSION 2.6)
project (RMF)

set (RMF_VERSION_MAJOR 1)
set (RMF_VERSION_MINOR 0)



include_directories ("${PROJECT_SOURCE_DIR}/include")
include_directories ("${PROJECT_BIN_DIR}/include")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/include/RMF/config.h.in"
  "${PROJECT_BINARY_DIR}/include/RMF/config.h"
  )

FILE(GLOB lib_sources src/*.cpp src/internal/*.cpp )
add_library(RMF ${lib_sources})


FILE(GLOB headers_0 include/RMF/*.h}
FILE(GLOB headers_1 include/RMF/internal/*.h)

install (TARGETS RMF DESTINATION lib)
install (FILES ${headers_0} DESTINATION include/RMF)
install (FILES ${headers_1} DESTINATION include/RMF/internal)


# bin
FILE(GLOB bins bin/*.cpp)
foreach (b ${bins})
  GET_FILENAME_COMPONENT(name, ${b}, NAME_WE)
  add_executable(${name} ${b})
endforeach(b)

# tests
FILE(GLOB python_tests test/test_*.py)
foreach (t ${python_tests})
  add_test(${t} ${t})
  set_tests_properties(${t} RETURN 0)
endforeach(t)
