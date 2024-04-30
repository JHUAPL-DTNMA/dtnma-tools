/*
 * Copyright (c) 2011-2023 The Johns Hopkins University Applied Physics
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
#include <osapi-task.h>
#include <osapi-error.h>
#include <osapi-common.h>
#include <shared/utils/daemon_run.h>
#include <shared/adm/adm.h>
#include <shared/msg/msg_if.h>
#include <agent/instr.h>
#include <agent/ldc.h>
#include <agent/rda.h>
#include <agent/ingest.h>
#include <unity.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdatomic.h>

/// Agent state for all tests
static nmagent_t agent;

static atomic_int test_count;
static sem_t      test_done;

/** A simple EDD
 */
tnv_t *_test_edd_5(tnvc_t *params)
{
    return tnv_from_real32(1.5);
}

/** A control that produces a report if given a parameter.
 */
tnv_t *_test_ctrl(eid_t *def_mgr, tnvc_t *params, int8_t *status)
{
    --test_count;
    printf("Called ctrl! Remaining: %d\n", test_count);
    if (test_count <= 0)
    {
        sem_post(&test_done);
    }

    *status = CTRL_SUCCESS;
    if (!params || (vec_num_entries(params->values) == 0))
    {
        printf("No parameters\n");
        return NULL;
    }
    tnv_t *param0 = vec_at(&(params->values), 0);
    int    success;
    printf("Parameter 0: %d\n", tnv_to_int(*param0, &success));
    return tnv_copy_ptr(param0);
}

static int _test_send(const blob_t *data, const eid_t *dest, void *ctx)
{
    printf("Called send to %s!\n", dest);
    --test_count;
    if (test_count <= 0)
    {
        sem_post(&test_done);
    }
    return AMP_OK;
}

blob_t *rx_blob;
// Lock for #rx_blob
sem_t rx_blob_write;
// Lock for #rx_blob
sem_t rx_blob_read;

static blob_t *_test_receive(msg_metadata_t *meta, daemon_run_t *running, int *success, void *ctx)
{
    printf("Called receive!\n");
    blob_t *res = NULL;
    if (sem_wait(&rx_blob_read))
    {
        *success = AMP_SYSERR;
        return NULL;
    }
    res     = rx_blob;
    rx_blob = NULL;
    if (sem_post(&rx_blob_write))
    {
        *success = AMP_SYSERR;
        return NULL;
    }

    if (res)
    {
        --test_count;
    }
    else
    {
        sem_post(&test_done);
    }

    *success = (res ? AMP_OK : AMP_FAIL);
    return res;
}

void setUp(void)
{
    TEST_ASSERT_TRUE(nmagent_init(&agent));
    agent_instr_init();

    test_count = 0;
    TEST_ASSERT_EQUAL_INT(0, sem_init(&test_done, 0, 0));
    TEST_ASSERT_EQUAL_INT(0, sem_init(&rx_blob_write, 0, 1));
    TEST_ASSERT_EQUAL_INT(0, sem_init(&rx_blob_read, 0, 0));

    printf("Defining test ADM\n");
    {
        ari_t *id = adm_build_ari(AMP_TYPE_EDD, false, 12, 5);
        TEST_ASSERT_NOT_NULL(id);
        int ret = adm_add_edd(id, _test_edd_5);
        TEST_ASSERT_EQUAL_INT(AMP_OK, ret);
    }
    {
        ari_t *id = adm_build_ari(AMP_TYPE_CTRL, true, 12, 34);
        TEST_ASSERT_NOT_NULL(id);
        int ret = adm_add_ctrldef_ari(id, 1, _test_ctrl);
        TEST_ASSERT_EQUAL_INT(AMP_OK, ret);
    }

    agent.mif = (mif_cfg_t) { .send = _test_send, .receive = _test_receive, .ctx = NULL };
    TEST_ASSERT_EQUAL_INT(AMP_OK, rda_init());
}

void tearDown(void)
{
    rda_cleanup();
    nmagent_destroy(&agent);

    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&rx_blob_read));
    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&rx_blob_write));
    TEST_ASSERT_EQUAL_INT(0, sem_destroy(&test_done));

    adm_common_destroy();
    // Sdr sdr = getIonsdr();
    // sdr_uncatlg(sdr, "nmagent_db");

    db_destroy();
    utils_mem_teardown();
}

void test_rda_ctrls(void)
{
    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_lock(&(gAgentDb.rpt_msgs.lock)));
    TEST_ASSERT_EQUAL_INT(0, vec_num_entries(gAgentDb.rpt_msgs));
    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_unlock(&(gAgentDb.rpt_msgs.lock)));

    pthread_t thr;
    TEST_ASSERT_EQUAL_INT(0, pthread_create(&thr, NULL, rda_ctrls, &agent));
    TEST_ASSERT_EQUAL_INT(OS_SUCCESS, OS_TaskDelay(1));
    test_count = 1;

    // Inject the run-control directly after thread start
    ari_t *id = adm_build_ari(AMP_TYPE_CTRL, true, 12, 34);

    ctrl_t *ctrl = ctrl_create(id);
    ari_release(id, true);
    TEST_ASSERT_NOT_NULL(ctrl);
    ctrl->start = OS_TimeFromTotalSeconds(0);
    TEST_ASSERT_EQUAL_INT(RH_OK, VDB_ADD_CTRL(ctrl, NULL));

    // wait for completion
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 10;
    TEST_ASSERT_EQUAL_INT(0, sem_timedwait(&test_done, &timeout));
    TEST_ASSERT_EQUAL_INT(0, test_count);

    daemon_run_stop(&agent.running);
    rda_signal_shutdown();
    TEST_ASSERT_EQUAL_INT(NULL, pthread_join(thr, NULL));

    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_lock(&(gAgentDb.rpt_msgs.lock)));
    TEST_ASSERT_EQUAL_INT(0, vec_num_entries(gAgentDb.rpt_msgs));
    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_unlock(&(gAgentDb.rpt_msgs.lock)));
}

void test_rda_ctrls_report(void)
{
    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_lock(&(gAgentDb.rpt_msgs.lock)));
    TEST_ASSERT_EQUAL_INT(0, vec_num_entries(gAgentDb.rpt_msgs));
    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_unlock(&(gAgentDb.rpt_msgs.lock)));

    pthread_t thr;
    TEST_ASSERT_EQUAL_INT(0, pthread_create(&thr, NULL, rda_ctrls, &agent));
    TEST_ASSERT_EQUAL_INT(OS_SUCCESS, OS_TaskDelay(1));
    test_count = 1;

    // Inject the run-control directly after thread start
    ari_t *id = adm_build_ari(AMP_TYPE_CTRL, true, 12, 34);
    ari_add_parm_val(id, tnv_from_int(567));

    ctrl_t *ctrl = ctrl_create(id);
    ari_release(id, true);
    TEST_ASSERT_NOT_NULL(ctrl);
    strncpy(ctrl->caller.name, "dtn:none", AMP_MAX_EID_LEN);
    ctrl->start = OS_TimeFromTotalSeconds(0);
    TEST_ASSERT_EQUAL_INT(RH_OK, VDB_ADD_CTRL(ctrl, NULL));

    // wait for completion
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 10;
    TEST_ASSERT_EQUAL_INT(0, sem_timedwait(&test_done, &timeout));
    TEST_ASSERT_EQUAL_INT(0, test_count);

    daemon_run_stop(&agent.running);
    rda_signal_shutdown();
    TEST_ASSERT_EQUAL_INT(NULL, pthread_join(thr, NULL));

    // report message left in queue
    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_lock(&(gAgentDb.rpt_msgs.lock)));
    TEST_ASSERT_EQUAL_INT(1, vec_num_entries(gAgentDb.rpt_msgs));
    {
        msg_rpt_t *msg = vec_at(&(gAgentDb.rpt_msgs), 0);
        TEST_ASSERT_NOT_NULL(msg);
        TEST_ASSERT_EQUAL_INT(1, vec_num_entries(msg->rpts));
        rpt_t *rpt = vec_at(&(msg->rpts), 0);
        TEST_ASSERT_NOT_NULL(rpt);
        TEST_ASSERT_EQUAL_INT(1, vec_num_entries(rpt->entries->values));
        tnv_t *val = vec_at(&(rpt->entries->values), 0);
        TEST_ASSERT_NOT_NULL(val);
        TEST_ASSERT_EQUAL(AMP_TYPE_INT, val->type);
        int success;
        TEST_ASSERT_EQUAL_INT(567, tnv_to_int(*val, &success));
        TEST_ASSERT_EQUAL_INT(1, success);
    }
    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_unlock(&(gAgentDb.rpt_msgs.lock)));
}

void test_rda_rules_tbr(void)
{
    pthread_t thr;
    TEST_ASSERT_EQUAL_INT(0, pthread_create(&thr, NULL, rda_rules, &agent));
    TEST_ASSERT_EQUAL_INT(OS_SUCCESS, OS_TaskDelay(1));
    test_count = 2;

    // Define the TBR after thread start
    ari_t    *id    = adm_build_ari(AMP_TYPE_TBR, false, 12, 56);
    OS_time_t start = OS_TimeFromTotalSeconds(0);
    tbr_def_t def;
    def.period   = OS_TimeFromTotalSeconds(1);
    def.max_fire = test_count;

    ac_t action;
    ac_init(&action);
    ac_insert(&action, adm_build_ari(AMP_TYPE_CTRL, true, 12, 34));

    rule_t *tbr = rule_create_tbr(*id, start, def, action);
    ari_release(id, true);
    TEST_ASSERT_NOT_NULL(tbr);
    TEST_ASSERT_EQUAL_INT(RH_OK, VDB_ADD_RULE(&(tbr->id), tbr));

    // wait for completion
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 10;
    TEST_ASSERT_EQUAL_INT(0, sem_timedwait(&test_done, &timeout));
    TEST_ASSERT_EQUAL_INT(0, test_count);

    daemon_run_stop(&agent.running);
    rda_signal_shutdown();
    TEST_ASSERT_EQUAL_INT(NULL, pthread_join(thr, NULL));
}

void test_rda_reports(void)
{
    pthread_t thr;
    TEST_ASSERT_EQUAL_INT(0, pthread_create(&thr, NULL, rda_reports, &agent));
    TEST_ASSERT_EQUAL_INT(OS_SUCCESS, OS_TaskDelay(1));
    test_count = 1;

    // Inject the report directly after thread start
    eid_t recip;
    strncpy(recip.name, "dtn:none", AMP_MAX_EID_LEN);
    msg_rpt_t *msg_rpt = rda_get_msg_rpt(recip);
    TEST_ASSERT_NOT_NULL(msg_rpt);

    printf("Adding report\n");
    ari_t *id = adm_build_ari(AMP_TYPE_RPT, false, 12, 78);
    TEST_ASSERT_NOT_NULL(id);
    rpt_t *rpt = rpt_create(id, OS_TimeFromTotalSeconds(0), NULL);
    TEST_ASSERT_NOT_NULL(rpt);
    tnv_t *val = tnv_from_int(567);
    TEST_ASSERT_NOT_NULL(val);
    TEST_ASSERT_EQUAL_INT(AMP_OK, rpt_add_entry(rpt, val));
    TEST_ASSERT_EQUAL_INT(AMP_OK, msg_rpt_add_rpt(msg_rpt, rpt));

    pthread_mutex_lock(&gAgentDb.rpt_msgs.lock);
    pthread_cond_signal(&gAgentDb.rpt_msgs.cond_ins_mod);
    pthread_mutex_unlock(&gAgentDb.rpt_msgs.lock);

    // wait for completion
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 10;
    TEST_ASSERT_EQUAL_INT(0, sem_timedwait(&test_done, &timeout));
    TEST_ASSERT_EQUAL_INT(0, test_count);

    daemon_run_stop(&agent.running);
    rda_signal_shutdown();
    TEST_ASSERT_EQUAL_INT(NULL, pthread_join(thr, NULL));
}

void test_ldc_edd_value(void)
{
    ari_t *id = adm_build_ari(AMP_TYPE_EDD, false, 12, 5);
    TEST_ASSERT_NOT_NULL(id);
    tnv_t *val = ldc_collect(id, NULL);
    TEST_ASSERT_NOT_NULL(val);

    TEST_ASSERT_EQUAL_INT(AMP_TYPE_REAL32, val->type);
    int   success;
    float cval = tnv_to_real32(*val, &success);
    TEST_ASSERT_EQUAL_INT(1, success);
    TEST_ASSERT_FLOAT_WITHIN(1.5, cval, 1e-6);
}

void test_rx_thread(void)
{
    pthread_t thr;
    TEST_ASSERT_EQUAL_INT(0, pthread_create(&thr, NULL, rx_thread, &agent));
    TEST_ASSERT_EQUAL_INT(OS_SUCCESS, OS_TaskDelay(1));
    test_count = 1;

    // Inject the run-control directly after thread start
    TEST_ASSERT_EQUAL_INT(0, sem_wait(&rx_blob_write));
    rx_blob = utils_string_to_hex("820052020081c115410605022523818a186b410200");
    TEST_ASSERT_EQUAL_INT(0, sem_post(&rx_blob_read));

    // no data here
    TEST_ASSERT_EQUAL_INT(0, sem_wait(&rx_blob_write));
    TEST_ASSERT_EQUAL_INT(0, sem_post(&rx_blob_read));

    // wait for completion
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 10000;
    TEST_ASSERT_EQUAL_INT(0, sem_timedwait(&test_done, &timeout));
    TEST_ASSERT_EQUAL_INT(0, test_count);

    daemon_run_stop(&agent.running);
    rda_signal_shutdown();
    TEST_ASSERT_EQUAL_INT(NULL, pthread_join(thr, NULL));

    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_lock(&(gAgentDb.rpt_msgs.lock)));
    TEST_ASSERT_EQUAL_INT(0, vec_num_entries(gAgentDb.rpt_msgs));
    TEST_ASSERT_EQUAL_INT(0, pthread_mutex_unlock(&(gAgentDb.rpt_msgs.lock)));
}
