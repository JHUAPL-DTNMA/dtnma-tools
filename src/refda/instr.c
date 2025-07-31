/*
 * Copyright (c) 2011-2025 The Johns Hopkins University Applied Physics
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
/** @file
 * @ingroup refda
 * Agent Instrumentation definitions.
 */
#include "instr.h"
#include <cace/util/defs.h>

void refda_instr_init(refda_instr_t *obj)
{
    atomic_init(&(obj->num_execset_recv), 0);
    atomic_init(&(obj->num_execset_recv_failure), 0);
    atomic_init(&(obj->num_rptset_sent), 0);
    atomic_init(&(obj->num_rptset_sent_failure), 0);
    atomic_init(&(obj->num_tbrs), 0);
    atomic_init(&(obj->num_tbrs_trig), 0);
    atomic_init(&(obj->num_sbrs), 0);
    atomic_init(&(obj->num_sbrs_trig), 0);
    atomic_init(&(obj->num_ctrls_run), 0);
}

void refda_instr_deinit(refda_instr_t *obj _U_)
{
    // no corresponding clear functions
}
