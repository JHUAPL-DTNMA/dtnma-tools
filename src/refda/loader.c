/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
 * Laboratory LLC.
 *
 * This file is part of the Delay-Tolerant Networking Management
 * Architecture (DTNMA) Tools package.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
