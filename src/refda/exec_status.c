#include "exec_status.h"
#include <cace/util/defs.h>
#include <cace/util/logging.h>
#include <stddef.h>

void refda_exec_status_init(refda_exec_status_t *obj)
{
    CHKVOID(obj);
    sem_init(&obj->finished, 0, 0);
    atomic_store(&obj->failed, false);
    obj->on_finished     = NULL;
    obj->on_finished_arg = NULL;
}

void refda_exec_status_deinit(refda_exec_status_t *obj)
{
    CHKVOID(obj);
    obj->on_finished_arg = NULL;
    obj->on_finished     = NULL;
    sem_destroy(&obj->finished);
}

bool refda_exec_status_wait(refda_exec_status_t *obj)
{
    CHKRET(obj, true);
    sem_wait(&obj->finished);
    return atomic_load(&obj->failed);
}

void refda_exec_status_post(refda_exec_status_t *obj, bool failed)
{
    atomic_store(&obj->failed, failed);
    if (obj->on_finished)
    {
        (obj->on_finished)(failed, obj->on_finished_arg);
    }
    // post after all caller-thread work done
    sem_post(&obj->finished);
}
