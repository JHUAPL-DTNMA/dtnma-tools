#include "semtype_cnst.h"

void amm_semtype_cnst_init(amm_semtype_cnst_t *obj)
{
    CHKVOID(obj);
    obj->type = AMM_SEMTYPE_CNST_INVALID;
}

void amm_semtype_cnst_deinit(amm_semtype_cnst_t *obj)
{
    CHKVOID(obj);
    switch (obj->type)
    {
        case AMM_SEMTYPE_CNST_INVALID:
            break;
        case AMM_SEMTYPE_CNST_STRLEN:
            cace_amm_range_size_deinit(&(obj->as_strlen));
            break;
        case AMM_SEMTYPE_CNST_RANGE_INT64:
            cace_amm_range_int64_deinit(&(obj->as_range_int64));
            break;
    }
    obj->type = AMM_SEMTYPE_CNST_INVALID;
}

cace_amm_range_size_t *amm_semtype_cnst_set_strlen(amm_semtype_cnst_t *obj)
{
    CHKNULL(obj);
    amm_semtype_cnst_deinit(obj);

    obj->type                   = AMM_SEMTYPE_CNST_STRLEN;
    cace_amm_range_size_t *cnst = &(obj->as_strlen);
    cace_amm_range_size_init(cnst);

    return cnst;
}

bool amm_semtype_cnst_is_valid(const amm_semtype_cnst_t *obj, const ari_t *val)
{
    bool retval = false;
    switch (obj->type)
    {
        case AMM_SEMTYPE_CNST_INVALID:
            break;
        case AMM_SEMTYPE_CNST_STRLEN:
        {
            const cace_amm_range_size_t *cnst = &(obj->as_strlen);
            if (val->is_ref)
            {
                return false;
            }
            size_t len = 0;
            switch (val->as_lit.prim_type)
            {
                case ARI_PRIM_TSTR:
                    // ignore terminating null
                    len = ari_cget_tstr(val)->len - 1;
                    break;
                case ARI_PRIM_BSTR:
                    len = ari_cget_bstr(val)->len;
                    break;
                default:
                    return false;
            }
            retval = cace_amm_range_size_contains(cnst, len);
            break;
        }
        case AMM_SEMTYPE_CNST_RANGE_INT64:
        {
            int64_t intval;
            if (ari_get_vast(val, &intval))
            {
                return false;
            }
            const cace_amm_range_int64_t *cnst = &(obj->as_range_int64);
            retval                             = cace_amm_range_int64_contains(cnst, intval);
            break;
        }
    }
    return retval;
}
