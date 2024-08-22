#include "edd.h"
#include "cace/util.h"

void cace_amm_edd_desc_init(cace_amm_edd_desc_t *obj)
{
    cace_amm_obj_desc_init(&(obj->base));
    obj->typeobj = NULL;
    obj->produce = NULL;
}

void cace_amm_edd_desc_deinit(cace_amm_edd_desc_t *obj)
{
    amm_type_deinit(obj->typeobj);
    cace_amm_obj_desc_deinit(&(obj->base));
    // not necessary but helpful
    memset(obj, 0, sizeof(*obj));
}

int cace_amm_edd_desc_produce(const cace_amm_edd_desc_t *obj, cace_amm_valprod_ctx_t *ctx)
{
    CHKERR1(obj);
    CHKERR1(ctx);
    CHKERR1(obj->typeobj);
    CHKERR1(obj->produce);

    (obj->produce)(obj, ctx);

    if (ari_is_undefined(&(ctx->value)))
    {
        return 2;
    }

    // force output type
    ari_t tmp;
    ari_init(&tmp);
    int res = amm_type_convert(obj->typeobj, &tmp, &(ctx->value));
    ari_set_move(&(ctx->value), &tmp);
    if (res)
    {
        return 3;
    }

    return 0;
}
