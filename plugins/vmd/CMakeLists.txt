set(VMDPLUGIN_INCLUDE_DIR ${VMDPLUGIN_INCLUDE_DIR} CACHE STRING "Path to vmd plugin headers")

find_path(VMDPLUGIN_INCLUDE_PATH
  NAMES molfile_plugin.h
  PATHS ${VMDPLUGIN_INCLUDE_DIR}
)

if(${VMDPLUGIN_INCLUDE_PATH} MATCHES "VMDPLUGIN_INCLUDE_PATH-NOTFOUND")
message(STATUS "Compilation of VMD plugin disabled. Defined VMDPLUGIN_INCLUDE_DIR to point to the include path for the header.")
else()
include_directories(${VMDPLUGIN_INCLUDE_PATH})

add_library(rmfplugin SHARED rmfplugin.cpp)

target_link_libraries(rmfplugin
                      RMF
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${HDF5_LIBRARIES}
  ${Log4CXX_LIBRARY}
  )
endif()