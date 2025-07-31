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
 * @ingroup refdm
 * Agent Instrumentation declarations.
 */
#ifndef REFDM_INSTR_H_
#define REFDM_INSTR_H_

#include <m-atomic.h>

/** Instrumentation counters for a Manager.
 */
typedef struct
{
    /// Count of EXECSET values sent to any Agent
    atomic_ullong num_execset_sent;
    /// Count of EXECSET values failed to send
    atomic_ullong num_execset_sent_failure;
    /// Count of RPTSET values received from any Agent
    atomic_ullong num_rptset_recv;
} refdm_instr_t;

/** Initialize counters to zero.
 */
void refdm_instr_init(refdm_instr_t *obj);

/** Reset counters to zero.
 */
void refdm_instr_deinit(refdm_instr_t *obj);

#endif /* REFDM_INSTR_H_ */
