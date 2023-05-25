# Shared library for agent and manager
set(HFILES
  "shared/nm.h"
  "shared/platform.h"
  "shared/adm/adm.h"
  "shared/platform.h"
  "shared/utils/cbor_utils.h"
  "shared/utils/db.h"
  "shared/utils/daemon_run.h"
  "shared/utils/nm_types.h"
  "shared/utils/rhht.h"
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
  "shared/msg/msg.h"
  "shared/msg/msg_if.h"
)
set(CFILES
  "shared/adm/adm.c"
  "shared/adm/adm_init.c"
  "shared/utils/cbor_utils.c"
  "shared/utils/daemon_run.c"
  "shared/utils/nm_types.c"
  "shared/utils/db.c"
  "shared/utils/rhht.c"
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
  "shared/msg/msg.c"
  "shared/msg/msg_if.c"
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
target_link_libraries(nmcommon PUBLIC MLIB::mlib)
target_link_libraries(nmcommon PUBLIC qcbor)

set(HFILES
  agent/ingest.h
  agent/instr.h
  agent/lcc.h
  agent/ldc.h
  agent/nmagent.h
  agent/rda.h
)
set(CFILES
  agent/ingest.c
  agent/instr.c
  agent/lcc.c
  agent/ldc.c
  agent/nmagent.c
  agent/rda.c
)
add_library(nmagent ${CFILES} ${HFILES})
target_compile_definitions(nmagent PRIVATE _GNU_SOURCE)
target_link_libraries(nmagent PUBLIC nmcommon)
target_include_directories(nmagent PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/agent
)

set(HFILES
  "ion_if/ion_if.h"
)
set(CFILES
  "ion_if/ion_if.c"
  ion_if/nmagent.c
)
add_executable(nm_agent ${CFILES} ${HFILES})
target_link_libraries(nm_agent nmagent)
target_link_libraries(nm_agent ION::BP)

install(
  TARGETS nm_agent
  RUNTIME
)
