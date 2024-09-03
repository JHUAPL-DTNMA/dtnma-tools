#include "msg_if.h"

void cace_amm_msg_if_metadata_init(cace_amm_msg_if_metadata_t *meta)
{
    cace_data_init(&(meta->src));
    cace_data_init(&(meta->dest));
}

void cace_amm_msg_if_metadata_deinit(cace_amm_msg_if_metadata_t *meta)
{
    cace_data_deinit(&(meta->dest));
    cace_data_deinit(&(meta->src));
}
