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
#include "runctx.h"
#include "cace/util/defs.h"

void refda_amm_modval_state_init(refda_amm_modval_state_t *obj)
{
    obj->_ctr             = 0;
    obj->notify           = NULL;
    obj->notify_user_data = NULL;
}

void refda_amm_modval_state_deinit(refda_amm_modval_state_t *obj)
{
    // no real cleanup
    memset(obj, 0, sizeof(*obj));
}

void refda_amm_modval_state_inc(refda_amm_modval_state_t *obj)
{
    obj->_ctr += 1;

    if (obj->notify)
    {
        (obj->notify)(obj, obj->notify_user_data);
    }
}

int refda_amm_exec_ctx_init(refda_amm_exec_ctx_t *obj, const cace_amm_formal_param_list_t fparams, const ari_t *ref)
{
    CHKERR1(obj)
    CHKERR1(fparams)
    CHKERR1(ref)
    CHKERR1(ref->is_ref)

    obj->objpath = &(ref->as_ref.objpath);
    cace_amm_actual_param_set_init(&(obj->aparams));
    ari_init(&(obj->result));

    int res = cace_amm_actual_param_set_populate(&(obj->aparams), fparams, &(ref->as_ref.params));
    if (res)
    {
        return 2;
    }

    return 0;
}

void refda_amm_exec_ctx_deinit(refda_amm_exec_ctx_t *obj)
{
    ari_deinit(&(obj->result));
    cace_amm_actual_param_set_deinit(&(obj->aparams));
}

int refda_amm_valprod_ctx_init(refda_amm_valprod_ctx_t *obj, const cace_amm_formal_param_list_t fparams,
                               const ari_t *ref)
{
    CHKERR1(obj)
    CHKERR1(fparams)
    CHKERR1(ref)
    CHKERR1(ref->is_ref)

    obj->objpath = &(ref->as_ref.objpath);
    cace_amm_actual_param_set_init(&(obj->aparams));
    ari_init(&(obj->value));

    int res = cace_amm_actual_param_set_populate(&(obj->aparams), fparams, &(ref->as_ref.params));
    if (res)
    {
        return 2;
    }

    return 0;
}

void refda_amm_valprod_ctx_deinit(refda_amm_valprod_ctx_t *obj)
{
    ari_deinit(&(obj->value));
    cace_amm_actual_param_set_deinit(&(obj->aparams));
}
