include_directories("${PROJECT_BINARY_DIR}/src"
  "${PROJECT_SOURCE_DIR}/src")

include("Files.cmake")
include("internal/Files.cmake")

include(backend/CMakeLists.txt)

add_library(RMF-lib SHARED ${sources})
add_dependencies(RMF-lib RMF-decorators)
set_target_properties("RMF-lib" PROPERTIES OUTPUT_NAME "RMF")
set_property(TARGET "RMF-lib" PROPERTY FOLDER "RMF")
set_property(TARGET "RMF-lib" PROPERTY COMPILE_DEFINITIONS RMF_EXPORT)

target_link_libraries(RMF-lib
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${HDF5_LIBRARIES}
  ${Log4CXX_LIBRARY}
  )

INSTALL(TARGETS RMF-lib DESTINATION ${CMAKE_INSTALL_LIBDIR})
