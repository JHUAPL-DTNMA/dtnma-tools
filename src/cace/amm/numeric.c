#include "numeric.h"
#include "cace/util/defs.h"

static ari_type_t eqiv_ari_type(const ari_lit_t *lit)
{
    if (lit->has_ari_type)
    {
        return lit->ari_type;
    }

    switch (lit->prim_type)
    {
        case ARI_PRIM_UINT64:
        {
            const uint64_t *val = &(lit->value.as_uint64);
            if (*val <= UINT8_MAX)
            {
                return ARI_TYPE_BYTE;
            }
            if ((*val <= UINT32_MAX))
            {
                return ARI_TYPE_UINT;
            }
            return ARI_TYPE_UVAST;
        }
        case ARI_PRIM_INT64:
        {
            const int64_t *val = &(lit->value.as_int64);
            if ((*val >= INT32_MIN) && (*val <= INT32_MAX))
            {
                return ARI_TYPE_INT;
            }
            return ARI_TYPE_VAST;
        }
        case ARI_PRIM_FLOAT64:
            return ARI_TYPE_REAL64;
        default:
            break;
    }

    return ARI_TYPE_NULL;
}

static int numeric_rank(ari_type_t typ)
{
    switch (typ)
    {
        case ARI_TYPE_BYTE:
            return 0;
        case ARI_TYPE_UINT:
            return 1;
        case ARI_TYPE_INT:
            return 2;
        case ARI_TYPE_UVAST:
            return 3;
        case ARI_TYPE_VAST:
            return 4;
        case ARI_TYPE_REAL32:
            return 5;
        case ARI_TYPE_REAL64:
            return 6;
        default:
            return -1;
    }
}

int cace_amm_numeric_promote_type(ari_type_t *result, const ari_t *left, const ari_t *right)
{
    CHKERR1(result);
    CHKERR1(left);
    CHKERR1(right);
    CHKERR1(!(left->is_ref));
    CHKERR1(!(right->is_ref));

    ari_type_t lt_typ = eqiv_ari_type(&(left->as_lit));
    ari_type_t rt_typ = eqiv_ari_type(&(right->as_lit));

    // promotion is symmetric, so swap to make logic more simple
    if (numeric_rank(lt_typ) > numeric_rank(rt_typ))
    {
        M_SWAP(ari_type_t, lt_typ, rt_typ);
    }

    if ((lt_typ == ARI_TYPE_INT) && (rt_typ == ARI_TYPE_UVAST))
    {
        *result = ARI_TYPE_VAST;
    }
    else
    {
        // higher-rank wins
        *result = rt_typ;
    }
    return 0;
}
