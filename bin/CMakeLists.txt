
FILE(GLOB BIN_SOURCES "*.cpp")
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/bin)
foreach (cpp ${BIN_SOURCES})
   GET_FILENAME_COMPONENT(name ${cpp} NAME_WE)
   add_executable(RMF.${name} ${cpp})
   target_link_libraries(RMF.${name} RMF ${Boost_LIBRARIES} )
   set_target_properties(RMF.${name} PROPERTIES
                         OUTPUT_NAME ${name})
   set_property(TARGET "RMF.${name}" PROPERTY FOLDER "RMF")
   INSTALL(TARGETS RMF.${name} DESTINATION ${CMAKE_INSTALL_BINDIR})
   set(bins ${bins} RMF.${name})
endforeach()

set(RMF_BINS ${bins} CACHE INTERNAL "" FORCE)