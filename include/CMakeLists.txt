FILE(GLOB headers_0 RMF/*.h)
FILE(GLOB headers_1 RMF/internal/*.h)

add_custom_command (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/RMF.h
  COMMAND MakeTable ${CMAKE_CURRENT_BINARY_DIR}/Table.h
  DEPENDS MakeTable
  )

install (FILES ${headers_0} DESTINATION include/RMF)
install (FILES all_header DESTINATION include)
install (FILES ${headers_1} DESTINATION include/RMF/internal)
