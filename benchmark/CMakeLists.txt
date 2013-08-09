
FILE(GLOB BIN_SOURCES "benchmark/*.cpp")
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/bin)
foreach (cpp ${BIN_SOURCES})
   GET_FILENAME_COMPONENT(name ${cpp} NAME_WE)
   message(STATUS ${name})
   add_executable("RMF.${name}" ${cpp})
   target_link_libraries("RMF.${name}" RMF-lib ${Boost_PROGRAM_OPTIONS_LIBRARY} )
   set_target_properties(RMF.${name} PROPERTIES
                         OUTPUT_NAME ${name})
   set_property(TARGET "RMF.${name}" PROPERTY FOLDER "RMF")

   set(executables ${executables} "RMF.${name}")
endforeach()