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
#include "msgdata.h"
#include <cace/util/defs.h>

void refda_msgdata_init(refda_msgdata_t *obj)
{
    CHKVOID(obj);
    m_string_init(obj->ident);
    ari_init(&(obj->value));
}

void refda_msgdata_init_move(refda_msgdata_t *obj, refda_msgdata_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    m_string_init_move(obj->ident, src->ident);
    ari_init_move(&(obj->value), &(src->value));
}

void refda_msgdata_deinit(refda_msgdata_t *obj)
{
    CHKVOID(obj);
    ari_deinit(&(obj->value));
    m_string_clear(obj->ident);
}

void refda_msgdata_set(refda_msgdata_t *obj, const refda_msgdata_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    m_string_set(obj->ident, src->ident);
    ari_set_copy(&(obj->value), &(src->value));
}
