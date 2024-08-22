#include "runctx.h"
#include "parameters.h"
#include "cace/util.h"

int cace_amm_valprod_ctx_init(cace_amm_valprod_ctx_t *obj, const ari_formal_param_list_t fparams, const ari_t *ref)
{
    CHKERR1(obj);
    CHKERR1(fparams);
    CHKERR1(ref);
    CHKERR1(ref->is_ref);

    obj->objpath = &(ref->as_ref.objpath);
    ari_actual_param_set_init(&(obj->aparams));
    ari_init(&(obj->value));

    int res = ari_actual_param_set_populate(&(obj->aparams), fparams, &(ref->as_ref.params));
    if (res)
    {
        return 2;
    }

    return 0;
}

void cace_amm_valprod_ctx_deinit(cace_amm_valprod_ctx_t *obj)
{
    ari_deinit(&(obj->value));
    ari_actual_param_set_deinit(&(obj->aparams));
}
