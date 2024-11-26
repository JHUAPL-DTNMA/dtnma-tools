#include "exec_item.h"
#include <cace/util/defs.h>

void refda_exec_item_init(refda_exec_item_t *obj)
{
    CHKVOID(obj);
    ari_init(&(obj->ref));
    cace_amm_lookup_init(&(obj->deref));
}

void refda_exec_item_deinit(refda_exec_item_t *obj)
{
    CHKVOID(obj);
    cace_amm_lookup_deinit(&(obj->deref));
    ari_deinit(&(obj->ref));
}

void refda_exec_seq_init(refda_exec_seq_t *obj)
{
    CHKVOID(obj);
    refda_exec_item_list_init(obj->items);
}

void refda_exec_seq_deinit(refda_exec_seq_t *obj)
{
    CHKVOID(obj);
    refda_exec_item_list_clear(obj->items);
}
