/// Provide a name-mapping thunk between OSAL runtime and Unity test function
#include <osapi-common.h>
#include <osapi-bsp.h>

/// The entrypoint name created by unity
int unity_main(void);

/// The OSAL entrypoint signature
void OS_Application_Run(void)
{
  OS_BSP_SetExitCode(unity_main());
}
