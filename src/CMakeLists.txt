include_directories("${PROJECT_BINARY_DIR}/src"
  "${PROJECT_SOURCE_DIR}/src")

FILE(GLOB sources "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/internal/*.cpp")

include(src/backend/CMakeLists.txt)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/internal/paths.cpp.in"
  "${PROJECT_BINARY_DIR}/src/internal/paths.cpp"
  )

add_library(RMF SHARED ${sources}
  "${PROJECT_BINARY_DIR}/src/internal/paths.cpp" ${json_headers}
  ${decorators})

target_link_libraries(RMF
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${AvroCPP_LIBRARY}
  ${HDF5_LIBRARIES}
  )

INSTALL(TARGETS RMF DESTINATION ${CMAKE_INSTALL_LIBDIR})
