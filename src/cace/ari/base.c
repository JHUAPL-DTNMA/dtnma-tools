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
/** @file
 * @ingroup ari
 */
#include "base.h"
#include "lit.h"
#include "ref.h"
#include "containers.h"
#include "cace/util/defs.h"
#include <inttypes.h>

/** Reset the state of a value struct.
 *
 * @param ari The struct to reset.
 */
static void ari_state_reset(ari_t *ari)
{
    memset(ari, 0, sizeof(ari_t));
}

static void ari_copy_shallow(ari_t *ari, const ari_t *src)
{
    memcpy(ari, src, sizeof(ari_t));
}

static int ari_copy_deep(ari_t *ari, const ari_t *src)
{
    int retval = 0;
    if (src->is_ref)
    {
        retval = ari_ref_copy(ari_init_objref(ari), &(src->as_ref));
        CHKERRVAL(retval);
    }
    else
    {
        retval = ari_lit_copy(ari_init_lit(ari), &(src->as_lit));
        CHKERRVAL(retval);
    }
    return retval;
}

static void ari_deinit_parts(ari_t *ari)
{
    if (ari->is_ref)
    {
        ari_ref_deinit(&(ari->as_ref));
    }
    else
    {
        ari_lit_deinit(&(ari->as_lit));
    }
}

void ari_init(ari_t *ari)
{
    CHKVOID(ari);
    ari_state_reset(ari);
}

ari_lit_t *ari_init_lit(ari_t *ari)
{
    CHKNULL(ari);
    ari_state_reset(ari);
    ari->is_ref = false;
    return &(ari->as_lit);
}

ari_ref_t *ari_init_objref(ari_t *ari)
{
    CHKNULL(ari);
    ari_state_reset(ari);
    ari->is_ref = true;
    return &(ari->as_ref);
}

ari_ref_t *ari_set_objref(ari_t *ari)
{
    CHKNULL(ari);
    ari_deinit_parts(ari);
    return ari_init_objref(ari);
}

int ari_init_copy(ari_t *ari, const ari_t *src)
{
    CHKERR1(ari);
    CHKERR1(src);
    if (ari == src)
    {
        return 0;
    }

    ari_copy_deep(ari, src);
    return 0;
}

int ari_init_move(ari_t *ari, ari_t *src)
{
    CHKERR1(ari);
    CHKERR1(src);

    if (ari == src)
    {
        return 0;
    }

    ari_copy_shallow(ari, src);
    // reset the state of the src (not deinit)
    ari_state_reset(src);
    return 0;
}

int ari_deinit(ari_t *ari)
{
    CHKERR1(ari);
    ari_deinit_parts(ari);
    ari_state_reset(ari);
    return 0;
}

void ari_reset(ari_t *ari)
{
    CHKVOID(ari);
    ari_deinit_parts(ari);
    ari_state_reset(ari);
}

int ari_set_copy(ari_t *ari, const ari_t *src)
{
    CHKERR1(ari);
    CHKERR1(src);
    if (ari == src)
    {
        return 0;
    }

    ari_deinit_parts(ari);
    ari_copy_deep(ari, src);
    return 0;
}

int ari_set_move(ari_t *ari, ari_t *src)
{
    CHKERR1(ari);
    CHKERR1(src);
    if (ari == src)
    {
        return 0;
    }

    ari_deinit_parts(ari);
    ari_copy_shallow(ari, src);
    // reset the state of the src (not deinit)
    ari_state_reset(src);
    return 0;
}
