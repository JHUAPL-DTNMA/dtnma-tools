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
static void cace_ari_state_reset(cace_ari_t *ari)
{
    memset(ari, 0, sizeof(cace_ari_t));
}

static void cace_ari_copy_shallow(cace_ari_t *ari, const cace_ari_t *src)
{
    memcpy(ari, src, sizeof(cace_ari_t));
}

static int cace_ari_copy_deep(cace_ari_t *ari, const cace_ari_t *src)
{
    int retval = 0;
    if (src->is_ref)
    {
        retval = cace_ari_ref_copy(cace_ari_init_objref(ari), &(src->as_ref));
        CHKERRVAL(retval);
    }
    else
    {
        retval = cace_ari_lit_copy(cace_ari_init_lit(ari), &(src->as_lit));
        CHKERRVAL(retval);
    }
    return retval;
}

static void cace_ari_deinit_parts(cace_ari_t *ari)
{
    if (ari->is_ref)
    {
        cace_ari_ref_deinit(&(ari->as_ref));
    }
    else
    {
        cace_ari_lit_deinit(&(ari->as_lit));
    }
}

void cace_ari_init(cace_ari_t *ari)
{
    CHKVOID(ari);
    cace_ari_state_reset(ari);
}

cace_ari_lit_t *cace_ari_init_lit(cace_ari_t *ari)
{
    CHKNULL(ari);
    cace_ari_state_reset(ari);
    ari->is_ref = false;
    return &(ari->as_lit);
}

cace_ari_ref_t *cace_ari_init_objref(cace_ari_t *ari)
{
    CHKNULL(ari);
    cace_ari_state_reset(ari);
    ari->is_ref = true;
    return &(ari->as_ref);
}

cace_ari_ref_t *cace_ari_set_objref(cace_ari_t *ari)
{
    CHKNULL(ari);
    cace_ari_deinit_parts(ari);
    return cace_ari_init_objref(ari);
}

int cace_ari_init_copy(cace_ari_t *ari, const cace_ari_t *src)
{
    CHKERR1(ari);
    CHKERR1(src);
    if (ari == src)
    {
        return 0;
    }

    cace_ari_copy_deep(ari, src);
    return 0;
}

int cace_ari_init_move(cace_ari_t *ari, cace_ari_t *src)
{
    CHKERR1(ari);
    CHKERR1(src);

    if (ari == src)
    {
        return 0;
    }

    cace_ari_copy_shallow(ari, src);
    // reset the state of the src (not deinit)
    cace_ari_state_reset(src);
    return 0;
}

int cace_ari_deinit(cace_ari_t *ari)
{
    CHKERR1(ari);
    cace_ari_deinit_parts(ari);
    // reset is a convenience for postcondition
    cace_ari_state_reset(ari);
    return 0;
}

void cace_ari_reset(cace_ari_t *ari)
{
    CHKVOID(ari);
    cace_ari_deinit_parts(ari);
    cace_ari_state_reset(ari);
}

void cace_ari_set_copy(cace_ari_t *ari, const cace_ari_t *src)
{
    CHKVOID(ari);
    CHKVOID(src);
    if (ari == src)
    {
        // nothing to do
        return;
    }

    cace_ari_deinit_parts(ari);
    cace_ari_copy_deep(ari, src);
}

void cace_ari_set_move(cace_ari_t *ari, cace_ari_t *src)
{
    CHKVOID(ari);
    CHKVOID(src);
    if (ari == src)
    {
        // nothing to do
        return;
    }

    cace_ari_deinit_parts(ari);
    cace_ari_copy_shallow(ari, src);
    // reset the state of the src (not deinit)
    cace_ari_state_reset(src);
}
