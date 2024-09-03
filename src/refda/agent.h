
#ifndef REFDA_AGENT_H_
#define REFDA_AGENT_H_

#include <cace/util/daemon_run.h>
#include <cace/util/threadset.h>
#include <cace/amm/obj_ns.h>
#include <cace/amm/obj_store.h>
#include <cace/amm/msg_if.h>
#include <pthread.h>

/** State of a DTNMA Agent.
 */
typedef struct
{
    /// Agent endpoint ID
    string_t agent_eid;

    /// Running state
    daemon_run_t running;
    /// Messaging configuration
    cace_amm_msg_if_t mif;
    /// Threads associated with the agent
    threadset_t threads;

    /// Runtime AMM object store
    cace_amm_obj_store_t objs;
    /// Mutex for the state of #objs
    pthread_mutex_t objs_mutex;
} refda_agent_t;

void refda_agent_init(refda_agent_t *agent);

void refda_agent_deinit(refda_agent_t *agent);

int refda_agent_start(refda_agent_t *agent);

int refda_agent_stop(refda_agent_t *agent);

int refda_agent_send_hello(refda_agent_t *agent);

#endif /* REFDA_AGENT_H_ */
