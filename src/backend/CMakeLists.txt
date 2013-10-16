file(GLOB_RECURSE avrocpp_sources "${PROJECT_SOURCE_DIR}/src/backend/AvroCpp/impl/*.cc")

list(REMOVE_ITEM avrocpp_sources "${PROJECT_SOURCE_DIR}/src/backend/AvroCpp/impl/avrogencpp.cc")

add_definitions("-DAVRO_DYN_LINK" "-DAVRO_SOURCE")
include_directories("${PROJECT_SOURCE_DIR}/src/backend/AvroCpp/api")

list(APPEND cppfiles ${avrocpp_sources})


include(backend/avro/CMakeLists.txt)
