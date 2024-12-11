#include "itemized.h"
#include "cace/util/defs.h"

void cace_ari_itemized_init(cace_ari_itemized_t *obj)
{
    CHKVOID(obj);
    ari_array_init(obj->ordered);
    named_ari_ptr_dict_init(obj->named);
}

void cace_ari_itemized_init_move(cace_ari_itemized_t *obj, cace_ari_itemized_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    ari_array_init_move(obj->ordered, src->ordered);
    named_ari_ptr_dict_init_move(obj->named, src->named);
}

void cace_ari_itemized_deinit(cace_ari_itemized_t *obj)
{
    CHKVOID(obj);
    named_ari_ptr_dict_clear(obj->named);
    ari_array_clear(obj->ordered);
}

void cace_ari_itemized_reset(cace_ari_itemized_t *obj)
{
    CHKVOID(obj);
    ari_array_reset(obj->ordered);
    named_ari_ptr_dict_reset(obj->named);
}
