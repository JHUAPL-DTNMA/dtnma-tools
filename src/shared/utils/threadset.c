
#include <errno.h>
#include "threadset.h"
#include "debug.h"
#include "utils.h"

int threadset_start(list_thread_t *tset, const threadinfo_t *info, size_t count, void *arg)
{
  for (const threadinfo_t *it = info; it < info + count; ++it)
  {
    pthread_t thr;
    int res = pthread_create(&thr, NULL, it->func, arg);
    if (res)
    {
      AMP_DEBUG_ERR("threadset_start","Unable to create pthread %s, errno = %s",
                    it->name, strerror(errno));
      return AMP_SYSERR;
    }
    list_thread_push_back(*tset, thr);
    pthread_setname_np(thr, it->name);
  }

  return AMP_OK;
}

int threadset_join(list_thread_t *tset)
{
  while (!list_thread_empty_p(*tset))
  {
    pthread_t thr;
    list_thread_pop_back(&thr, *tset);
    if (pthread_join(thr, NULL))
    {
        AMP_DEBUG_ERR("threadset_join","Unable to join pthread %s, errno = %s",
                      "name", strerror(errno));
        return AMP_SYSERR;
    }
  }
  return AMP_OK;
}
