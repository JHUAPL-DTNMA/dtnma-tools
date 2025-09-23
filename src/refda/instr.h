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
 * Agent Instrumentation declarations.
 */
#ifndef REFDA_INSTR_H_
#define REFDA_INSTR_H_

#include <m-atomic.h>

/** Instrumentation counters for an Agent.
 */
typedef struct
{
    /// Count of EXECSET values received from any manager
    atomic_ullong num_execset_recv;
    /// Count of EXECSET values failed to receive
    atomic_ullong num_execset_recv_failure;
    /// Count of RPTSET values sent to any manager
    atomic_ullong num_rptset_sent;
    /// Count of RPTSET values failed to send
    atomic_ullong num_rptset_sent_failure;

    atomic_ullong num_tbrs;
    atomic_ullong num_tbrs_trig;
    atomic_ullong num_sbrs;
    atomic_ullong num_sbrs_trig;
    /// Count of individual CTRL run during execution
    atomic_ullong num_ctrls_run;
    atomic_ullong num_ctrls_succeeded;
    atomic_ullong num_ctrls_failed;

} refda_instr_t;

/** Initialize counters to zero.
 */
void refda_instr_init(refda_instr_t *obj);

/** Reset counters to zero.
 */
void refda_instr_deinit(refda_instr_t *obj);

#endif /* REFDA_INSTR_H_ */
