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
#include "obj_desc.h"

void cace_amm_user_data_init(cace_amm_user_data_t *obj)
{
    obj->ptr    = NULL;
    obj->owned  = false;
    obj->deinit = NULL;
}

void cace_amm_user_data_deinit(cace_amm_user_data_t *obj)
{
    if (obj->ptr)
    {
        if (obj->deinit)
        {
            (obj->deinit)(obj->ptr);
        }
        if (obj->owned)
        {
            CACE_FREE(obj->ptr);
        }
    }

    obj->deinit = NULL;
    obj->owned  = false;
    obj->ptr    = NULL;
}

void cace_amm_user_data_set_from(cace_amm_user_data_t *obj, void *ptr, bool owned, cace_amm_user_data_deinit_f deinit)
{
    cace_amm_user_data_deinit(obj);

    obj->ptr    = ptr;
    obj->owned  = owned;
    obj->deinit = deinit;
}

void cace_amm_obj_desc_init(cace_amm_obj_desc_t *obj)
{
    cace_amm_idseg_val_init(&obj->obj_id);
    cace_amm_formal_param_list_init(obj->fparams);
    cace_amm_user_data_init(&(obj->app_data));
}

void cace_amm_obj_desc_deinit(cace_amm_obj_desc_t *obj)
{
    cace_amm_user_data_deinit(&(obj->app_data));
    cace_amm_formal_param_list_clear(obj->fparams);
    cace_amm_idseg_val_deinit(&obj->obj_id);
}
