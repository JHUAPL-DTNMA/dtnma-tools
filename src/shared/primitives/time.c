#include "time.h"
#include "../utils/utils.h"
#include "../utils/cbor_utils.h"

int amp_tv_deserialize(QCBORDecodeContext *dec, amp_tv_t *item)
{
    CHKUSR(item, AMP_FAIL);
    return cut_get_cbor_numeric(dec, AMP_TYPE_TV, item);
}

int amp_tv_serialize(QCBOREncodeContext *enc, const amp_tv_t *item)
{
    CHKUSR(item, AMP_FAIL);
    QCBOREncode_AddUInt64(enc, OS_TimeGetTotalSeconds(item->secs));
    return AMP_OK;
}

/** Convert a possibly relative time into an absolute time.
 */
OS_time_t amp_tv_to_ctime(amp_tv_t val, const OS_time_t *event_time)
{
    OS_time_t res;
    if(OS_TimeGetTotalSeconds(val.secs) <= EPOCH_ABSTIME_DTN)
    {
        if (event_time)
        {
          res = OS_TimeAdd(*event_time, val.secs);
        }
        else
        {
            // Log error
            AMP_DEBUG_ERR("amp_tv_to_ctime", "Cannot convert a relative time without a reference event");
            res = OS_TimeAssembleFromMilliseconds(0, 0);
        }
    }
    else
    {
      res = OS_TimeAdd(OS_TimeAssembleFromMilliseconds(EPOCH_DTN_POSIX, 0), val.secs);
    }
    return res;
}

int TimeCompare(OS_time_t time1, OS_time_t time2)
{
  if (time1.ticks == time2.ticks)
  {
    return 0;
  }
  return time1.ticks < time2.ticks ? -1 : 1;
}

amp_tv_t amp_tv_from_ctime(OS_time_t val, const OS_time_t *event_time)
{
    amp_tv_t res;
    if (event_time)
    {
        if (TimeCompare(val, *event_time) < 0)
        {
            AMP_DEBUG_ERR("amp_tv_from_ctime", "Input time %lld is earlier than the event %lld", val, *event_time);
            res = AMP_TV_ZERO;
        }
        else
        {
            res.secs = OS_TimeSubtract(val, *event_time);
        }
    }
    else
    {
        const OS_time_t min_rel_time = OS_TimeAssembleFromMilliseconds(EPOCH_ABSTIME_DTN + EPOCH_DTN_POSIX, 0);
        if (TimeCompare(val, min_rel_time) < 0)
        {
            AMP_DEBUG_ERR("amp_tv_from_ctime", "Input time %lld is earlier than the epoch %lld", val, min_rel_time);
            res = AMP_TV_ZERO;
        }
        else
        {
            res.secs = OS_TimeSubtract(val, min_rel_time);
        }
    }
    return res;
}
