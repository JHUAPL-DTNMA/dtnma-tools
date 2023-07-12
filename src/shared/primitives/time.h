/*
 * Copyright (c) 2023 The Johns Hopkins University Applied Physics
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
#ifndef AMP_PRIMITIVES_TIME_H_
#define AMP_PRIMITIVES_TIME_H_

#include <osapi-clock.h>
#include <qcbor/qcbor.h>
#include "../utils/nm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Seconds to DTN epoch from the POSIX epoch
#define	EPOCH_DTN_POSIX 946684800

/** The AMP relative time cut-off, set as the first second of
 * September 9th, 2012 in the DTN epoch.
 */
#define EPOCH_ABSTIME_DTN 558230400

/// Timestamp (TS) value
typedef struct {
    /// Time from the DTN epoch
    OS_time_t secs;
} amp_tv_t;

int amp_tv_deserialize(QCBORDecodeContext *dec, amp_tv_t *item);

int amp_tv_serialize(QCBOREncodeContext *enc, const amp_tv_t *item);

#define AMP_TV_ZERO (amp_tv_t){0}

OS_time_t amp_tv_to_ctime(amp_tv_t val, const OS_time_t *event_time);

amp_tv_t amp_tv_from_ctime(OS_time_t val, const OS_time_t *event_time);

// Non-standard OS_time operators
#define OS_TIME_ZERO (OS_time_t){.ticks = 0}
#define OS_TIME_MAX (OS_time_t){.ticks = INT64_MAX}
int TimeCompare(OS_time_t time1, OS_time_t time2);
OS_time_t TimeMin(OS_time_t time1, OS_time_t time2);

struct timespec TimeToTimespec(OS_time_t time);

#ifdef __cplusplus
}
#endif

#endif /* AMP_PRIMITIVES_TIME_H_ */
