#include "semtype_cnst.h"

void amm_semtype_cnst_init(amm_semtype_cnst_t *obj)
{
    obj->type = AMM_SEMTYPE_CNST_INVALID;
}

void amm_semtype_cnst_deinit(amm_semtype_cnst_t *obj)
{
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

bool amm_semtype_cnst_is_valid(const amm_semtype_cnst_t *obj, const ari_t *val)
{
    switch (obj->type)
    {
        case AMM_SEMTYPE_CNST_INVALID:
            break;
        case AMM_SEMTYPE_CNST_STRLEN: {
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
                    len = ari_cget_tstr(val)->len;
                    break;
                default:
                    return false;
            }
            cace_amm_range_size_contains(cnst, len);
            break;
        }
        case AMM_SEMTYPE_CNST_RANGE_INT64: {
            int64_t intval;
            if (ari_get_vast(val, &intval))
            {
                return false;
            }
            const cace_amm_range_int64_t *cnst = &(obj->as_range_int64);
            cace_amm_range_int64_contains(cnst, intval);
            break;
        }
    }
    // invalid fall through
    return false;
}
