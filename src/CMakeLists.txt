include("Files.cmake")

file(GLOB_RECURSE avrocpp_sources "${PROJECT_SOURCE_DIR}/src/backend/AvroCpp/impl/*.cc")

list(REMOVE_ITEM avrocpp_sources "${PROJECT_SOURCE_DIR}/src/backend/AvroCpp/impl/avrogencpp.cc")

add_definitions("-DAVRO_DYN_LINK" "-DAVRO_SOURCE")
include_directories("${PROJECT_SOURCE_DIR}/src/backend/AvroCpp/api")



foreach (jf ${jsonfiles})
set(jsonfiles_expanded ${jsonfiles_expanded} ${PROJECT_SOURCE_DIR}/src/${jf})
endforeach()

add_custom_command(OUTPUT "${PROJECT_BINARY_DIR}/src/generated/embed_json.h" "${PROJECT_BINARY_DIR}/src/generated/embed_json.cpp"
   COMMAND python "${PROJECT_SOURCE_DIR}/scripts/make-embed.py"
              "${PROJECT_BINARY_DIR}/src/generated/" ${jsonfiles_expanded}
   DEPENDS ${jsonfiles} "${PROJECT_SOURCE_DIR}/scripts/make-embed.py"
   COMMENT "Embedding json files" VERBATIM  )

add_custom_target(RMF-embed DEPENDS "${PROJECT_BINARY_DIR}/src/generated/embed_json.cpp")

set_source_files_properties("${PROJECT_BINARY_DIR}/src/generated/embed_jsons.cpp" PROPERTIES GENERATED True)

add_library(RMF-lib SHARED ${cppfiles} ${avrocpp_sources} "${PROJECT_BINARY_DIR}/src/generated/embed_jsons.cpp")
add_definitions(-DRMF_EXPORTS)
add_dependencies(RMF-lib RMF-decorators RMF-embed)
set_target_properties("RMF-lib" PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      OUTPUT_NAME "RMF")
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
