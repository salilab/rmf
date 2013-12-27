
include("Files.cmake")
include_directories(${PROJECT_SOURCE_DIR}/bin)
foreach (cpp ${cppfiles})
   get_filename_component(name ${cpp} NAME_WE)
   message(STATUS ${name})
   add_executable("RMF.${name}" ${cpp})
   target_link_libraries("RMF.${name}" RMF-lib ${Boost_PROGRAM_OPTIONS_LIBRARY} )
   set_target_properties(RMF.${name} PROPERTIES
                         OUTPUT_NAME ${name})
   set_property(TARGET "RMF.${name}" PROPERTY FOLDER "RMF")

   set(executables ${executables} "RMF.${name}")
endforeach()
