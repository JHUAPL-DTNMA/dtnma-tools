/*
 * Copyright (c) 2011-2024 The Johns Hopkins University Applied Physics
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
#include "named_type.h"
#include "cace/util/defs.h"

void cace_amm_named_type_init(cace_amm_named_type_t *obj)
{
    CHKVOID(obj);
    m_string_init(obj->name);
    cace_amm_type_init(&(obj->typeobj));
}

void cace_amm_named_type_deinit(cace_amm_named_type_t *obj)
{
    CHKVOID(obj);
    cace_amm_type_deinit(&(obj->typeobj));
    m_string_clear(obj->name);
}
