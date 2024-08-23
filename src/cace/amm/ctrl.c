#include "ctrl.h"
#include "cace/logging.h"
#include "cace/ari/text.h"
#include "cace/util.h"

void cace_amm_ctrl_desc_init(cace_amm_ctrl_desc_t *obj)
{
    cace_amm_obj_desc_init(&(obj->base));
    amm_type_init(&(obj->typeobj));
    obj->execute = NULL;
}

void cace_amm_ctrl_desc_deinit(cace_amm_ctrl_desc_t *obj)
{
    amm_type_deinit(&(obj->typeobj));
    cace_amm_obj_desc_deinit(&(obj->base));
    // not necessary but helpful
    memset(obj, 0, sizeof(*obj));
}

int cace_amm_ctrl_desc_execute(const cace_amm_ctrl_desc_t *obj, cace_amm_exec_ctx_t *ctx)
{
    CHKERR1(obj)
    CHKERR1(ctx)
    CHKERR1(obj->execute)

    int res = (obj->execute)(obj, ctx);
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, &(ctx->result), ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("execution finished with status %d and result %s", res, string_get_cstr(buf));
        string_clear(buf);
    }
    if (res)
    {
        ari_set_undefined(&(ctx->result));
        return 2;
    }

    if (amm_type_is_valid(&(obj->typeobj)))
    {
        // force result type
        ari_t tmp;
        ari_init(&tmp);
        res = amm_type_convert(&(obj->typeobj), &tmp, &(ctx->result));
        ari_set_move(&(ctx->result), &tmp);
        if (res)
        {
            ari_set_undefined(&(ctx->result));
            return 3;
        }
    }
    else
    {
        // success is treated as a null value
        if (ari_is_undefined(&(ctx->result)))
        {
            ari_set_null(&(ctx->result));
        }

        if (!ari_is_null(&(ctx->result)))
        {
            // should not have a result
            ari_set_undefined(&(ctx->result));
            return 4;
        }
    }

    return 0;
}
