#include "loader.h"
#include "adm/ietf.h"
#include "adm/ietf_amm.h"
#include "adm/ietf_amm_base.h"
#include "adm/ietf_amm_semtype.h"
#include "adm/ietf_network_base.h"
#include "adm/ietf_dtnma_agent.h"
#include "adm/ietf_dtnma_agent_acl.h"
#include "adm/ietf_alarms.h"

int refda_loader_basemods(refda_agent_t *agent)
{
    int retval = 0;
    retval += refda_adm_ietf_amm_init(agent) == 0 ? 0 : 1;
    retval += refda_adm_ietf_amm_base_init(agent) == 0 ? 0 : 1;
    retval += refda_adm_ietf_amm_semtype_init(agent) == 0 ? 0 : 1;
    retval += refda_adm_ietf_network_base_init(agent) == 0 ? 0 : 1;
    retval += refda_adm_ietf_dtnma_agent_init(agent) == 0 ? 0 : 1;
    retval += refda_adm_ietf_dtnma_agent_acl_init(agent) == 0 ? 0 : 1;
    retval += refda_adm_ietf_alarms_init(agent) == 0 ? 0 : 1;
    return retval;
}
