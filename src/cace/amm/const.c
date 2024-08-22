#include "const.h"
#include "cace/util.h"

void cace_amm_const_desc_init(cace_amm_const_desc_t *obj)
{
    cace_amm_obj_desc_init(&(obj->base));
    obj->value = ARI_INIT_UNDEFINED;
}

void cace_amm_const_desc_deinit(cace_amm_const_desc_t *obj)
{
    cace_amm_obj_desc_deinit(&(obj->base));
    memset(obj, 0, sizeof(*obj));
}

int cace_amm_const_desc_produce(const cace_amm_const_desc_t *obj, cace_amm_valprod_ctx_t *ctx)
{
    CHKERR1(obj);
    CHKERR1(ctx);

    ari_set_copy(&(ctx->value), &(obj->value));
    // FIXME use ctx parameters to substitute
    return 0;
}
