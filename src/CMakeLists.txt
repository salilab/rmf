FILE(GLOB lib_sources *.cpp internal/*.cpp )
add_library(RMF SHARED ${lib_sources})
target_link_libraries(RMF hdf5 )

target_link_libraries(RMF ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

install (TARGETS RMF DESTINATION lib)
