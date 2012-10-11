cmake_minimum_required(VERSION 2.8)
project(RMF)

# needs to be in main CMakeLists.txt
ENABLE_TESTING()

find_package(Boost COMPONENTS system filesystem thread program_options
REQUIRED)

include(GNUInstallDirs)

include(FindHDF5)
find_package(HDF5 REQUIRED)

include(FindProtobuf)
find_package(Protobuf)
include_directories(${PROTOBUF_INCLUDE_DIR})
#lib_directories(${PROTOBUF_LIB_DIR})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})

set (RMF_VERSION_MAJOR 0)
set (RMF_VERSION_MINOR 9)

if (PROTOBUF_FOUND)
  set(PROTOBUF_FOUND_INT 1)
  PROTOBUF_GENERATE_CPP(protobuf_source protobuf_header "data/RMF.proto")
else(PROTOBUF_FOUND)
  set(PROTOBUF_FOUND_INT 0)
endif(PROTOBUF_FOUND)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/include/RMF/config.h"
  )


FILE(GLOB SOURCES "src/*.cpp" "src/internal/*.cpp")
FILE(GLOB HEADERS "include/RMF/*.h" "include/internal/RMF/*.h")

include_directories("${PROJECT_BINARY_DIR}/include"
"${PROJECT_SOURCE_DIR}/include" ${HDF5_INCLUDE_DIRS} "${PROJECT_BINARY_DIR}")

add_library(RMF SHARED ${SOURCES} ${protobuf_source}
${protobuf_header} ${HEADERS})

target_link_libraries(RMF
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${PROTOBUF_LIBRARY}
    ${HDF5_LIBRARIES}
)

if (DEFINED CMAKE_INSTALL_PYTHONDIR)
  message(STATUS "CMAKE_INSTALL_PYTHONDIR defined as ${CMAKE_INSTALL_PYTHONDIR}")
else()
  set(CMAKE_INSTALL_PYTHONDIR "${CMAKE_INSTALL_LIBDIR}/python/site_packages")
endif()

ADD_SUBDIRECTORY(bin)
ADD_SUBDIRECTORY(pyext)
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(benchmark)
ADD_SUBDIRECTORY(doc)

INSTALL(TARGETS RMF DESTINATION ${CMAKE_INSTALL_LIBDIR})
INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/include/RMF  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/include/RMF  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
