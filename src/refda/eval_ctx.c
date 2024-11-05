#include "eval_ctx.h"
#include "cace/util/defs.h"

void refda_eval_ctx_init(refda_eval_ctx_t *obj, refda_runctx_t *parent)
{
    CHKVOID(obj);
    obj->parent = parent;
    obj->deref  = NULL;
    refda_eval_list_init(obj->input);
    ari_list_init(obj->stack);
}

void refda_eval_ctx_deinit(refda_eval_ctx_t *obj)
{
    CHKVOID(obj);
    ari_list_clear(obj->stack);
    refda_eval_list_clear(obj->input);
    obj->deref  = NULL;
    obj->parent = NULL;
}
