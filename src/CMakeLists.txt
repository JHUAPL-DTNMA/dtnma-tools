# Shared library for agent and manager
set(HFILES
  "shared/nm.h"
  "shared/platform.h"
  "shared/utils/utils.h"
  "shared/utils/vector.h"
  "shared/primitives/ari.h"
  "shared/primitives/blob.h"
  "shared/primitives/ctrl.h"
  "shared/primitives/edd_var.h"
  "shared/primitives/expr.h"
  "shared/primitives/report.h"
  "shared/primitives/rules.h"
  "shared/primitives/table.h"
  "shared/primitives/time.h"
  "shared/primitives/tnv.h"
)
set(CFILES
  "shared/utils/utils.c"
  "shared/utils/vector.c"
  "shared/primitives/ari.c"
  "shared/primitives/blob.c"
  "shared/primitives/ctrl.c"
  "shared/primitives/edd_var.c"
  "shared/primitives/expr.c"
  "shared/primitives/report.c"
  "shared/primitives/rules.c"
  "shared/primitives/table.c"
  "shared/primitives/time.c"
  "shared/primitives/tnv.c"
)
add_library(nmcommon ${CFILES} ${HFILES})
target_include_directories(nmcommon PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_definitions(nmcommon PUBLIC
  USE_MALLOC=1
)
target_link_libraries(nmcommon PUBLIC m)
target_link_libraries(nmcommon PUBLIC osal)
target_link_libraries(nmcommon PUBLIC mlib)
target_link_libraries(nmcommon PUBLIC qcbor)


file(GLOB_RECURSE CFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/*.cpp
)
#add_library(nm_agent ${CFILES} ${HFILES})
#target_link_libraries(nm_agent PUBLIC nmcommon)
#target_include_directories(nm_agent PUBLIC
#    ${CMAKE_CURRENT_SOURCE_DIR}/agent
#)
