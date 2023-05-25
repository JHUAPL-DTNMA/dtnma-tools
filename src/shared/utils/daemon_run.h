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

extern int daemon_run_init(daemon_run_t *dr);
extern void daemon_run_cleanup(daemon_run_t *dr);
extern void daemon_run_stop(daemon_run_t *dr);
extern bool daemon_run_get(daemon_run_t *dr);

#ifdef __cplusplus
}
#endif

#endif /* _DAEMON_RUN_H_ */
