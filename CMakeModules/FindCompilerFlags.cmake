if(APPLE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
# to support profiling on mac os
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreFoundation")
list(REMOVE_DUPLICATES CMAKE_CXX_FLAGS)
list(REMOVE_DUPLICATES CMAKE_EXE_LINKER_FLAGS)
endif(APPLE)


if (APPLE)
  execute_process(COMMAND uname -v OUTPUT_VARIABLE DARWIN_VERSION)
  string(REGEX MATCH "[0-9]+" DARWIN_VERSION ${DARWIN_VERSION})
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE
		        GCC_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "GCC version: ${GCC_VERSION}")
if (GCC_VERSION VERSION_GREATER 4.6 OR GCC_VERSION VERSION_EQUAL 4.6)
message(STATUS "No conversion issue")
elseif (GCC_VERSION VERSION_GREATER 4.3 OR GCC_VERSION VERSION_EQUAL 4.3)
add_definitions("-Wno-conversion")
endif()
endif()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND "${CMAKE_BUILD_TYPE}" MATCHES "Debug")
  if (NOT APPLE OR DARWIN_VERSION GREATER 12)
    add_definitions("-sanitize=undefined,address")
    add_definitions("-fno-sanitize-recover")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sanitize=undefined,address")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sanitize=undefined,address")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sanitize=undefined,address")
  else()
    message(STATUS "Old clang")
  endif()
endif()