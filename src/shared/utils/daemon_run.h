#ifndef _DAEMON_RUN_H_
#define _DAEMON_RUN_H_

#include <stdbool.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

/** State for daemon run/stop signalling.
 */
typedef struct daemon_run {
  sem_t stop;
} daemon_run_t;

int daemon_run_init(daemon_run_t *dr);
void daemon_run_cleanup(daemon_run_t *dr);
void daemon_run_stop(daemon_run_t *dr);
bool daemon_run_get(daemon_run_t *dr);

/** Wait until the running state is stopped.
 * @param dr The run object.
 * @return True if successful.
 */
bool daemon_run_wait(daemon_run_t *dr);

#ifdef __cplusplus
}
#endif

#endif /* _DAEMON_RUN_H_ */
