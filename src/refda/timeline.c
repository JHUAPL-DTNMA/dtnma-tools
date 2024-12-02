#include "timeline.h"
#include <timespec.h>

int refda_timeline_event_cmp(const refda_timeline_event_t *lt, const refda_timeline_event_t *rt)
{
    if (!lt)
    {
        return 1;
    }
    if (!rt)
    {
        return -1;
    }
    return timespec_cmp(lt->ts, rt->ts);
}
