#include "named_type.h"
#include "cace/util/defs.h"

void amm_named_type_init(amm_named_type_t *obj)
{
    CHKVOID(obj);
    m_string_init(obj->name);
    amm_type_init(&(obj->typeobj));
}

void amm_named_type_deinit(amm_named_type_t *obj)
{
    CHKVOID(obj);
    amm_type_deinit(&(obj->typeobj));
    m_string_clear(obj->name);
}
