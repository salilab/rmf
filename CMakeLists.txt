cmake_minimum_required(VERSION 2.8)
project(RMF)

find_package(Boost COMPONENTS system filesystem thread REQUIRED)

include(FindHDF5)
find_package(HDF5 REQUIRED)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})
#lib_directories(${PROTOBUF_LIB_DIR})

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})



set (RMF_VERSION_MAJOR 0)
set (RMF_VERSION_MINOR 9)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/include/RMF/config.h"
  )


FILE(GLOB SOURCES "src/*.cpp" "src/internal/*.cpp")
FILE(GLOB HEADERS "include/RMF/*.h" "include/internal/RMF/*.h")


add_library(RMF SHARED ${SOURCES} ${HEADERS})

target_link_libraries(RMF
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${PROTOBUF_LIBRARY}
    ${HDF5_LIBRARIES}
)


SET(CMAKE_SWIG_FLAGS "-castmode" "-naturalvar" "-Wextra" "-fvirtual" "-interface" "_RMF"
"-DPySwigIterator=RMF_PySwigIterator" "-c++"
"-DSwigPyIterator=RMF_SwigPyIterator" "-I${PROJECT_SOURCE_DIR}/pyext" "-DRMFEXPORT")

include_directories("${PROJECT_BINARY_DIR}/include"
"${PROJECT_SOURCE_DIR}/include" ${HDF5_INCLUDE_DIRS})

SET_SOURCE_FILES_PROPERTIES(RMF.i PROPERTIES CPLUSPLUS ON)
#SET_SOURCE_FILES_PROPERTIES(RMF.i PROPERTIES SWIG_FLAGS )
SWIG_ADD_MODULE(RMFPython python RMF.i)
SWIG_LINK_LIBRARIES(RMFPython ${PYTHON_LIBRARIES} RMF)
