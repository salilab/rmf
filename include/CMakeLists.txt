
set(decorators "${PROJECT_BINARY_DIR}/include/RMF/physics_decorators.h"
  "${PROJECT_BINARY_DIR}/include/RMF/sequence_decorators.h"
  "${PROJECT_BINARY_DIR}/include/RMF/shape_decorators.h"
  "${PROJECT_BINARY_DIR}/include/RMF/external_decorators.h"
  "${PROJECT_BINARY_DIR}/include/RMF/feature_decorators.h"
  "${PROJECT_BINARY_DIR}/include/RMF/alias_decorators.h"
  "${PROJECT_BINARY_DIR}/include/RMF/publication_decorators.h")

add_custom_command(OUTPUT ${decorators}
  COMMAND "python" "${PROJECT_SOURCE_DIR}/scripts/make-decorators.py"
  DEPENDS "${PROJECT_SOURCE_DIR}/scripts/make-decorators.py" "${PROJECT_SOURCE_DIR}/scripts/_decorators.py"
  WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
  COMMENT "Making decorator headers")
add_custom_target(RMF.decorators ALL DEPENDS "${PROJECT_BINARY_DIR}/include/RMF/physics_decorators.h"
  "${PROJECT_SOURCE_DIR}/scripts/make-decorators.py")
set_property(TARGET "RMF.decorators" PROPERTY FOLDER "RMF")

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/include/RMF  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/include/RMF  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
