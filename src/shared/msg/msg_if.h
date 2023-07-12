/*
 * Copyright (c) 202-2023 The Johns Hopkins University Applied Physics
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
 ** File Name: msg_if.h
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
 **  06/30/16  E. Birrane     Doc. Updates (Secure DTN - NASA: NNX14CS58P)
 **  10/02/18  E. Birrane     Update to AMP v0.5 (JHUAPL)
 *****************************************************************************/
#ifndef MSG_IF_H_
#define MSG_IF_H_

#include "msg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * +--------------------------------------------------------------------------+
 * |							  CONSTANTS  								  +
 * +--------------------------------------------------------------------------+
 */


/*
 * +--------------------------------------------------------------------------+
 * |							  DATA TYPES  								  +
 * +--------------------------------------------------------------------------+
 */

/** Message sending function.
 * @param data The data to send.
 * @param dest The destination EID.
 * @param ctx The user context, which may be NULL.
 * @return AMP_OK if successful.
 */
typedef int (*mif_send_t)(const blob_t *data, const eid_t *dest, void *ctx);

/** Message receiving function.
 * @param meta Pointer to reception metadata, which is never NULL.
 * @param success Result success indicator, which is never NULL.
 * If the result is not AMP_OK it is a signal to the reader to stop reading.
 * @param ctx The user context, which may be NULL.
 * @return A non-null pointer if successful.
 */
typedef blob_t * (*mif_receive_t)(msg_metadata_t *meta, int *success, void *ctx);

/**
 * The MSG Interface structure captures state necessary to communicate with
 * the local Bundle Protocol Agent (BPA).
 */
typedef struct
{
  mif_send_t send;
  mif_receive_t receive;
  /// Context to provide to #send and #receive functions
  void *ctx;
} mif_cfg_t;


/*
 * +--------------------------------------------------------------------------+
 * |						  DATA DEFINITIONS  							  +
 * +--------------------------------------------------------------------------+
 */


/*
 * +--------------------------------------------------------------------------+
 * |						  FUNCTION PROTOTYPES  							  +
 * +--------------------------------------------------------------------------+
 */

blob_t *mif_receive(mif_cfg_t *cfg, msg_metadata_t *meta, int *success);
int     mif_send_grp(mif_cfg_t *cfg, const msg_grp_t *group, const eid_t *destination);
int     mif_send_msg(mif_cfg_t *cfg, int msg_type, const void *msg, const eid_t *destination, amp_tv_t timestamp);

#ifdef __cplusplus
}
#endif

#endif /* MSG_IF_H_ */
