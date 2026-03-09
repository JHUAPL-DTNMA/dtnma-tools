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
#include "agent.h"
#include "ingress.h"
#include "egress.h"
#include "exec.h"
#include "reporting.h"
#include "amm/typedef.h"
#include "adm/ietf.h"
#include "adm/ietf_amm_base.h"
#include "adm/ietf_dtnma_agent.h"
#include "adm/ietf_dtnma_agent_acl.h"
#include "binding.h"
#include "cace/ari/text.h"
#include "cace/amm/lookup.h"
#include "cace/util/threadset.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"
#include <errno.h>

void refda_agent_init(refda_agent_t *agent)
{
    m_string_init(agent->agent_eid);
    cace_daemon_run_init(&(agent->running));
    refda_instr_init(&(agent->instr));
    cace_threadset_init(agent->threads);

    refda_acl_init(&(agent->acl));
    pthread_mutex_init(&(agent->acl_mutex), NULL);

    refda_alarms_init(&(agent->alarms));

    string_list_init(agent->odm_names);
    cace_amm_obj_store_init(&(agent->objs));
    pthread_mutex_init(&(agent->objs_mutex), NULL);

    refda_msgdata_queue_init(agent->execs, AGENT_QUEUE_SIZE);
    atomic_store(&agent->execs_enable, false);
    sem_init(&(agent->execs_sem), 0, 0);

    agent->exec_next_pid = 1;
    refda_exec_seq_list_init(agent->exec_state);
    pthread_mutex_init(&(agent->exec_state_mutex), NULL);
    refda_timeline_init(agent->exec_timeline);
    atomic_store(&agent->exec_end, false);

    refda_msgdata_queue_init(agent->rptgs, AGENT_QUEUE_SIZE);
    sem_init(&(agent->rptgs_sem), 0, 0);
}

void refda_agent_deinit(refda_agent_t *agent)
{
    sem_destroy(&(agent->rptgs_sem));
    refda_msgdata_queue_clear(agent->rptgs);

    refda_timeline_clear(agent->exec_timeline);
    pthread_mutex_destroy(&(agent->exec_state_mutex));
    refda_exec_seq_list_clear(agent->exec_state);
    agent->exec_next_pid = 0;

    sem_destroy(&(agent->execs_sem));
    // ignore execs_enable
    refda_msgdata_queue_clear(agent->execs);

    pthread_mutex_destroy(&(agent->objs_mutex));
    cace_amm_obj_store_deinit(&(agent->objs));
    string_list_clear(agent->odm_names);

    refda_alarms_deinit(&(agent->alarms));

    pthread_mutex_destroy(&(agent->acl_mutex));
    refda_acl_deinit(&(agent->acl));

    refda_instr_deinit(&(agent->instr));
    cace_threadset_clear(agent->threads);
    cace_daemon_run_cleanup(&(agent->running));
    m_string_clear(agent->agent_eid);
}

/// Time of the DTN epoch (2000-01-01T00:00:00Z) in the POSIX clock
#define DTN_EPOCH_IN_POSIX 946702800

int refda_agent_nowtime(refda_agent_t *agent _U_, cace_ari_t *val)
{
    CHKERR1(val);

    struct timespec time;
    int             res = clock_gettime(CLOCK_REALTIME, &time);
    if (res)
    {
        CACE_LOG_ERR("Failed in clock_gettime() with errno %d", errno);
        return 2;
    }

    // simple offset
    time.tv_sec -= DTN_EPOCH_IN_POSIX;

    cace_ari_set_tp(val, time);
    return 0;
}

cace_amm_obj_desc_t *refda_agent_get_object(refda_agent_t *agent, cace_ari_int_id_t org_id, cace_ari_int_id_t model_id,
                                            cace_ari_type_t type_id, cace_ari_int_id_t obj_id)
{
    cace_amm_obj_desc_t *found = NULL;

    cace_ari_t ref = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_objref_path_intid(&ref, org_id, model_id, type_id, obj_id);

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);

    int res = cace_amm_lookup_deref(&deref, &(agent->objs), &ref);
    if (res)
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, &ref, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_WARNING("Lookup failed with status %d for reference %s", res, m_string_get_cstr(buf));
        m_string_clear(buf);
    }
    else
    {
        found = deref.obj;
    }

    cace_amm_lookup_deinit(&deref);
    cace_ari_deinit(&ref);

    return found;
}

cace_amm_type_t *refda_agent_get_typedef(refda_agent_t *agent, cace_ari_int_id_t org_id, cace_ari_int_id_t model_id,
                                         cace_ari_int_id_t obj_id)
{
    cace_amm_obj_desc_t *obj = refda_agent_get_object(agent, org_id, model_id, CACE_ARI_TYPE_TYPEDEF, obj_id);

    refda_amm_typedef_desc_t *typedesc = obj ? obj->app_data.ptr : NULL;

    cace_amm_type_t *typeobj = typedesc ? &(typedesc->typeobj) : NULL;

    return typeobj;
}

int refda_agent_bindrefs(refda_agent_t *agent)
{
    REFDA_AGENT_LOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    int failcnt = 0;

    agent->mac_type = refda_agent_get_typedef(agent, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_AMM_BASE_ENUM_ADM,
                                              REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_MAC);
    if (!agent->mac_type)
    {
        ++failcnt;
    }

    agent->expr_type = refda_agent_get_typedef(agent, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_AMM_BASE_ENUM_ADM,
                                               REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_EXPR);
    if (!agent->expr_type)
    {
        ++failcnt;
    }

    agent->rptt_type = refda_agent_get_typedef(agent, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_AMM_BASE_ENUM_ADM,
                                               REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_RPTT);
    if (!agent->rptt_type)
    {
        ++failcnt;
    }

    agent->acl.perm_base =
        refda_agent_get_object(agent, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_ADM, CACE_ARI_TYPE_IDENT,
                               REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_PERMISSION);
    if (!agent->acl.perm_base)
    {
        ++failcnt;
    }

    agent->acl.perm_produce =
        refda_agent_get_object(agent, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_ADM, CACE_ARI_TYPE_IDENT,
                               REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_PRODUCE);
    if (!agent->acl.perm_produce)
    {
        ++failcnt;
    }

    if (failcnt)
    {
        CACE_LOG_WARNING("agent required type binding failures: %d", failcnt);
    }

    cace_amm_obj_ns_list_it_t ns_it;
    for (cace_amm_obj_ns_list_it(ns_it, agent->objs.ns_list); !cace_amm_obj_ns_list_end_p(ns_it);
         cace_amm_obj_ns_list_next(ns_it))
    {
        cace_amm_obj_ns_ptr_t *ns_ptr = *cace_amm_obj_ns_list_ref(ns_it);
        cace_amm_obj_ns_t     *ns     = cace_amm_obj_ns_ptr_ref(ns_ptr);
        CACE_LOG_DEBUG("Binding namespace ari:/%s/%s/", m_string_get_cstr(ns->org_id.name),
                       m_string_get_cstr(ns->model_id.name));

        refda_binding_ctx_t ctx = {
            .store = &(agent->objs),
            .ns    = ns,
        };

        cace_amm_obj_ns_ctr_dict_it_t objtype_it;
        for (cace_amm_obj_ns_ctr_dict_it(objtype_it, ns->object_types); !cace_amm_obj_ns_ctr_dict_end_p(objtype_it);
             cace_amm_obj_ns_ctr_dict_next(objtype_it))
        {
            cace_amm_obj_ns_ctr_dict_itref_t *pair = cace_amm_obj_ns_ctr_dict_ref(objtype_it);

            cace_ari_type_t        obj_type = pair->key;
            cace_amm_obj_ns_ctr_t *obj_ctr  = cace_amm_obj_ns_ctr_ptr_ref(pair->value);

            cace_amm_obj_desc_list_it_t obj_it;
            for (cace_amm_obj_desc_list_it(obj_it, obj_ctr->obj_list); !cace_amm_obj_desc_list_end_p(obj_it);
                 cace_amm_obj_desc_list_next(obj_it))
            {
                cace_amm_obj_desc_t *obj = cace_amm_obj_desc_ptr_ref(*cace_amm_obj_desc_list_ref(obj_it));

                const int objfailcnt = refda_binding_obj(&ctx, obj_type, obj);
                if (objfailcnt)
                {
                    CACE_LOG_WARNING("binding object ari:/%s/%s/%s/%s; failures %d", m_string_get_cstr(ns->org_id.name),
                                     m_string_get_cstr(ns->model_id.name), cace_ari_type_to_name(obj_type),
                                     m_string_get_cstr(obj->obj_id.name), objfailcnt);
                }
                failcnt += objfailcnt;
            }
        }
    }

    REFDA_AGENT_UNLOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);

    CACE_LOG_INFO("binding finished with %d failures", failcnt);
    return failcnt;
}

int refda_agent_init_sbr(refda_agent_t *agent, const cace_amm_obj_ns_t *ns)
{
    const cace_ari_type_t             obj_type = CACE_ARI_TYPE_SBR;
    cace_amm_obj_ns_ctr_ptr_t *const *ctr_ptr  = cace_amm_obj_ns_ctr_dict_get(ns->object_types, obj_type);
    if (!ctr_ptr)
    {
        return 1;
    }
    const cace_amm_obj_ns_ctr_t *ctr = cace_amm_obj_ns_ctr_ptr_ref(*ctr_ptr);

    cace_amm_obj_desc_list_it_t obj_it;
    for (cace_amm_obj_desc_list_it(obj_it, ctr->obj_list); !cace_amm_obj_desc_list_end_p(obj_it);
         cace_amm_obj_desc_list_next(obj_it))
    {
        const cace_amm_obj_desc_t *obj = cace_amm_obj_desc_ptr_ref(*cace_amm_obj_desc_list_cref(obj_it));
        refda_amm_sbr_desc_t      *sbr = obj->app_data.ptr;
        if (sbr)
        {
            if (sbr->init_enabled)
            {
                CACE_LOG_INFO("Initializing SBR");
                refda_exec_sbr_enable(agent, sbr);
            }
        }
    }

    return 0;
}

int refda_agent_init_tbr(refda_agent_t *agent, const cace_amm_obj_ns_t *ns)
{
    const cace_ari_type_t             obj_type = CACE_ARI_TYPE_TBR;
    cace_amm_obj_ns_ctr_ptr_t *const *ctr_ptr  = cace_amm_obj_ns_ctr_dict_get(ns->object_types, obj_type);
    if (!ctr_ptr)
    {
        return 1;
    }
    const cace_amm_obj_ns_ctr_t *ctr = cace_amm_obj_ns_ctr_ptr_ref(*ctr_ptr);

    cace_amm_obj_desc_list_it_t obj_it;
    for (cace_amm_obj_desc_list_it(obj_it, ctr->obj_list); !cace_amm_obj_desc_list_end_p(obj_it);
         cace_amm_obj_desc_list_next(obj_it))
    {
        const cace_amm_obj_desc_t *obj = cace_amm_obj_desc_ptr_ref(*cace_amm_obj_desc_list_cref(obj_it));
        refda_amm_tbr_desc_t      *tbr = obj->app_data.ptr;
        if (tbr)
        {
            if (tbr->init_enabled)
            {
                CACE_LOG_INFO("Initializing TBR");
                refda_exec_tbr_enable(agent, tbr);
            }
        }
    }

    return 0;
}

int refda_agent_init_objs(refda_agent_t *agent)
{
    REFDA_AGENT_LOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    cace_amm_obj_ns_list_it_t ns_it;

    for (cace_amm_obj_ns_list_it(ns_it, agent->objs.ns_list); !cace_amm_obj_ns_list_end_p(ns_it);
         cace_amm_obj_ns_list_next(ns_it))
    {
        cace_amm_obj_ns_ptr_t *const *ns_ptr = cace_amm_obj_ns_list_cref(ns_it);
        if (!ns_ptr)
        {
            continue;
        }
        const cace_amm_obj_ns_t *ns = cace_amm_obj_ns_ptr_ref(*ns_ptr);

        refda_agent_init_sbr(agent, ns);
        refda_agent_init_tbr(agent, ns);
    }

    REFDA_AGENT_UNLOCK(agent, REFDA_AGENT_ERR_LOCK_FAILED);
    return 0;
}

int refda_agent_start(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_INFO("Work threads starting...");

    /*
     * This following code only runs the ingress or egress threads if mif.recv and/or mif.send are defined.
     * This allows for short-cutting the ingress or egress threads and workers such that you can directly push into or
     * pop from the inter-thread queues. This is fully implemented for overwriting the ingress system using a call to
     * refda_ingress_push_move from a "external" thread.
     */
    size_t            threadCount = 0;
    cace_threadinfo_t threadinfo[3];
    if (agent->mif.recv)
    {
        threadinfo[threadCount].func   = &refda_ingress_worker;
        threadinfo[threadCount++].name = "ingress";
    }
    if (agent->mif.send)
    {
        threadinfo[threadCount].func   = &refda_egress_worker;
        threadinfo[threadCount++].name = "egress";
    }
    threadinfo[threadCount].func   = &refda_exec_worker;
    threadinfo[threadCount++].name = "exec";

    int res = cace_threadset_start(agent->threads, threadinfo, threadCount, agent);
    if (res)
    {
        CACE_LOG_ERR("Failed to start work threads: %d", res);
        return 2;
    }

    CACE_LOG_INFO("Work threads started");
    return 0;
}

int refda_agent_stop(refda_agent_t *agent)
{
    CHKERR1(agent);
    CACE_LOG_INFO("Work threads stopping...");

    // ensure this isn't blocking after startup failure
    atomic_store(&agent->execs_enable, true);

    // If the ingress system has been overwritten, then the undefined message needs to be
    // pushed in to signal shutdown.
    if (agent->mif.recv == NULL)
    {
        // Send sentinel to end thread execution
        refda_msgdata_t undef;
        refda_msgdata_init(&undef);
        refda_msgdata_queue_push_move(agent->execs, &undef);
        sem_post(&(agent->execs_sem));
    }

    /* Notify threads */
    cace_daemon_run_stop(&agent->running);

    cace_threadset_join(agent->threads);

    CACE_LOG_INFO("Work threads stopped");
    return 0;
}

int refda_agent_startup_exec(refda_agent_t *agent, cace_ari_t *target)
{
    CHKERR1(agent);
    CHKERR1(target);
    int retval = 0;

    refda_exec_status_t status;
    refda_exec_status_init(&status);

    refda_runctx_ptr_t *ctxptr = refda_runctx_ptr_new();
    refda_runctx_from(refda_runctx_ptr_ref(ctxptr), agent, NULL);
    CACE_LOG_DEBUG("Sending startup target");
    if (refda_exec_add_target(ctxptr, target, &status))
    {
        CACE_LOG_ERR("Failed adding startup target");
        retval = 3;
    }
    refda_runctx_ptr_clear(ctxptr);
    cace_ari_deinit(target);

    if (!retval)
    {
        bool failure = refda_exec_status_wait(&status);
        if (failure)
        {
            CACE_LOG_ERR("Failed executing startup target");
            retval = 3;
        }
        else
        {
            CACE_LOG_INFO("Finished startup target");
        }
    }
    refda_exec_status_deinit(&status);
    return retval;
}

void refda_agent_enable_exec(refda_agent_t *agent)
{
    CACE_LOG_INFO("Enabled execution ingress");
    atomic_store(&agent->execs_enable, true);
    sem_post(&agent->execs_sem);
}
