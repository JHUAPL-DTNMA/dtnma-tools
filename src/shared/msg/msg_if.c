/*
 * Copyright (c) 2011-2023 The Johns Hopkins University Applied Physics
 * Laboratory LLC.
 *
 * This file is part of the Delay-Tolerant Networking Management
 * Architecture (DTNMA) Tools package.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*****************************************************************************
 **
 ** File Name: msg_if.c
 **
 ** Description: This file contains the definitions, prototypes, constants, and
 **              other information necessary for DTNMP actors to connect to
 **              some messaging service.
 **
 ** Notes:
 **
 ** Assumptions:
 **
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR         DESCRIPTION
 **  --------  ------------   ---------------------------------------------
 **  08/10/11  V.Ramachandran Initial Implementation (JHU/APL)
 **  11/13/12  E. Birrane     Technical review, comment updates. (JHU/APL)
 **  06/25/13  E. Birrane     Renamed message "bundle" message "group". (JHU/APL)
 **  06/30/16  E. Birrane     Doc. Updates and Bug Fixes (Secure DTN - NASA: NNX14CS58P)
 **  10/02/18  E. Birrane     Update to AMP v0.5 (JHUAPL)
 *****************************************************************************/

#include <inttypes.h>
#include "../utils/nm_types.h"
#include "../utils/utils.h"
#include "msg.h"
#include "msg_if.h"


/******************************************************************************
 *
 * \par Function Name: mif_receive
 *
 * \par Blocking receive. Receives a message from the BPA.
 *
 * \retval NULL - Error
 *         !NULL - The received serialized payload.
 *
 * \param[out] meta    The sender information from the convergence layer for all msgs.
 * \param[in]  timeout The # seconds to wait on a receive before timing out
 *
 * \par Notes:
 *   - The returned data must be freed via zco_destroy_reference()
 *
 * \todo
 *   - Use ZCOs and handle large message sizes.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/10/11  V.Ramachandran Initial implementation (JHU/APL)
 *  10/04/18  E. Birrane     Updated to AMP v0.5 (JHU/APL)
 *****************************************************************************/

blob_t *mif_receive(mif_cfg_t *cfg, msg_metadata_t *meta, int *success)
{
  CHKNULL(cfg);
  CHKNULL(cfg->receive);
  CHKNULL(meta);
  CHKNULL(success);
  return (cfg->receive)(meta, success, cfg->ctx);
}


/******************************************************************************
 *
 * \par Function Name: mif_send
 *
 * \par Sends a text string to the recipient node.
 *
 * \retval Whether the send succeeded (1) or failed (0)
 *
 * \param[in] iif     The registered interface
 * \param[in] data    The data to send.
 * \param[in] len     The length of data to send, in bytes.
 * \param[in] rx_eid  The EID of the recipient of the data.
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/10/11  V.Ramachandran Initial implementation. (JHU/APL)
 *  06/25/13  E. Birrane     Renamed message "bundle" message "group". (JHU/APL)
 *  03/??/16  E. Birrane     Fix BP Send to latest ION version. (Secure DTN - NASA: NNX14CS58P)
 *  10/02/18  E. Birrane     Update to AMP v0.5 (JHU/APL)
 *****************************************************************************/

int mif_send_grp(mif_cfg_t *cfg, const msg_grp_t *group, const eid_t *destination)
{
    blob_t *data = NULL;

    CHKZERO(cfg);
    CHKZERO(cfg->send);
    CHKZERO(group);
    CHKZERO(destination);
    AMP_DEBUG_ENTRY("mif_send","(%p,%s)", group, destination->name);

    /* Step 1 - Serialize the bundle. */
    if((data = msg_grp_serialize_wrapper(group)) == NULL)
    {
    	AMP_DEBUG_ERR("mif_send","Bad message of length 0.", NULL);
    	AMP_DEBUG_EXIT("mif_send", "->0.", NULL);
    	return 0;
    }

    if(data->length == 0)
    {
    	AMP_DEBUG_ERR("mif_send","Cannot send empty data.", NULL);
    	blob_release(data, 1);
    	return AMP_FAIL;
    }

    /* Information on bitstream we are sending. */
    char *msg_str = utils_hex_to_string(data->value, data->length);
    AMP_DEBUG_ALWAYS("mif_send","Sending msgs:%s to %s:", msg_str, destination->name);
    SRELEASE(msg_str);

    (cfg->send)(data, destination, cfg->ctx);

    blob_release(data, 1);
    AMP_DEBUG_EXIT("mif_send", "->1.", NULL);
    return 1;
}

// Caller MUST release msg.
int mif_send_msg(mif_cfg_t *cfg, int msg_type, const void *msg, const eid_t *destination, amp_tv_t timestamp)
{
	msg_grp_t *grp = msg_grp_create(1);
	grp->timestamp = timestamp;
	int success;

	CHKERR(msg);
	CHKERR(grp);

	switch(msg_type)
	{
		case MSG_TYPE_REG_AGENT:
			success = msg_grp_add_msg_agent(grp, msg);
			break;
		case MSG_TYPE_RPT_SET:
			success = msg_grp_add_msg_rpt(grp, msg);
			break;
		case MSG_TYPE_PERF_CTRL:
			success = msg_grp_add_msg_ctrl(grp, msg);
			break;
		case MSG_TYPE_TBL_SET:
            success = msg_grp_add_msg_tbl(grp, msg);
            break;
		default:
			success = AMP_FAIL;
			break;
	}

	if(success == AMP_OK)
	{
            success = mif_send_grp(cfg, grp, destination);
	}

	msg_grp_release(grp, 1);
	return success;
}
