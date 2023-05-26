
#ifndef SRC_SHARED_UTILS_THREADSET_H_
#define SRC_SHARED_UTILS_THREADSET_H_

#include <pthread.h>
#include <m-list.h>

LIST_DEF(list_thread, pthread_t)

typedef struct {
  void* (*func)(void*);
  const char *name;
} threadinfo_t;

int threadset_start(list_thread_t *tset, const threadinfo_t *info, size_t count, void *arg);

int threadset_join(list_thread_t *tset);


#endif /* SRC_SHARED_UTILS_THREADSET_H_ */
