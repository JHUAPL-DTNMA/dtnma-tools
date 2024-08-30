#include "agent.h"

void refda_agent_init(refda_agent_t *agent)
{
    daemon_run_init(&(agent->running));
    threadset_init(agent->threads);
    cace_amm_obj_store_init(&(agent->objs));
    pthread_mutex_init(&(agent->objs_mutex), NULL);
}

void refda_agent_deinit(refda_agent_t *agent)
{
    pthread_mutex_destroy(&(agent->objs_mutex));
    cace_amm_obj_store_deinit(&(agent->objs));
    threadset_clear(agent->threads);
    daemon_run_cleanup(&(agent->running));
}
