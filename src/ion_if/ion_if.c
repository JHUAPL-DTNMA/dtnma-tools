/******************************************************************************
 **                           COPYRIGHT NOTICE
 **      (c) 2011 The Johns Hopkins University Applied Physics Laboratory
 **                         All rights reserved.
 ******************************************************************************/

/*****************************************************************************
 **
 ** File Name: ion_if.c
 **
 ** Description: This file contains the definitions, prototypes, constants, and
 **              other information necessary for DTNMP actors to connect to
 **              the local BPA.
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

#include "bp.h"

#include "shared/utils/nm_types.h"
#include "shared/utils/utils.h"
#include "shared/msg/msg.h"
#include "ion_if.h"



/******************************************************************************
 *
 * \par Function Name: iif_deregister_node
 *
 * \par Deregisters the current application with the DTN network.
 *
 * \retval 0 - Could not Register
 * 		   1 - Registered.
 *
 * \param[in,out] iif  The Interface being deregistered.
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/10/11  V.Ramachandran Initial implementation, (JHU/APL)
 *  06/30/16  E. Birrane     Fix EID init. (Secure DTN - NASA: NNX14CS58P)
 *****************************************************************************/

int iif_deregister_node(iif_t *iif)
{
    AMP_DEBUG_ENTRY("iif_deregister_node","(%#llx)", (size_t)iif);

    /* Step 0: Sanity Check */
    if(iif == NULL)
    {
    	AMP_DEBUG_ERR("iif_deregister_node","Null IIF.", NULL);
        AMP_DEBUG_EXIT("iif_deregister_node","-> %d", 0);
    	return 0;
    }

    bp_close(iif->sap);
    bp_detach();
    memset(iif->local_eid.name,0, AMP_MAX_EID_LEN);

    AMP_DEBUG_EXIT("iif_deregister_node","-> %d", 1);
    return 1;
}



/******************************************************************************
 *
 * \par Function Name: iif_get_local_eid
 *
 * \par Returns the EID of the local node.
 *
 * \retval The EID associated with the IIF.
 *
 * \param[in] iif  The Interface whose local EID is being queried.
 *
 * \par Notes:
 *         1. Assumes the IIF exists at this point.
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/10/11  V.Ramachandran Initial implementation. (JHU/APL)
 *****************************************************************************/

eid_t iif_get_local_eid(iif_t *iif)
{
	AMP_DEBUG_ENTRY("iif_get_local_eid","(%#llx)", iif);

	if(iif == NULL)
	{
		eid_t result;
		AMP_DEBUG_ERR("iif_get_local_eid","Bad args.",NULL);
		memset(&result,0,sizeof(eid_t));
		AMP_DEBUG_EXIT("iif_get_local_eid","->0.",NULL);
		return result;
	}

	AMP_DEBUG_EXIT("iif_get_local_eid","->1.",NULL);
    return iif->local_eid;
}



/******************************************************************************
 *
 * \par Function Name: iif_is_registered
 *
 * \par Returns 1 if the DTN connection is active, 0 otherwise.
 *
 * \retval 1 - IIF is registered
 *         0 - IIF is not registered.
 *
 * \param[in] iif  The Interface whose registration status is being queried.
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/10/11  V.Ramachandran Initial implementation. (JHU/APL)
 *****************************************************************************/

int iif_is_registered(iif_t *iif)
{
	uint8_t result = 0;

	AMP_DEBUG_ENTRY("iif_is_registered","(%#llx)", iif);

	if(iif == NULL)
	{
		AMP_DEBUG_ERR("iif_is_registered","Bad args.",NULL);
		AMP_DEBUG_EXIT("iif_is_registered","->0.",NULL);
		return result;
	}

	result = (iif->local_eid.name[0] != 0) ? 1 : 0;

	AMP_DEBUG_EXIT("iif_is_registered","->%d.",NULL);
    return result;
}




/******************************************************************************
 *
 * \par Function Name: iif_register_node
 *
 * \par Registers the current application with the DTN network.
 *
 * \retval 0 - Could not Register
 * 		   1 - Registered.
 *
 * \param[out] iif  Updated IIF structure.
 * \param[in]  eid  EID of the node we are registering.
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  08/10/11  V.Ramachandran Initial implementation . (JHU/APL)
 *****************************************************************************/

int iif_register_node(iif_t *iif, eid_t eid)
{
    AMP_DEBUG_ENTRY("iif_register_node","(%s)", eid.name);
    
    /* Step 0: Sanity Check */
    if(iif == NULL)
    {
    	AMP_DEBUG_ERR("iif_register_node","Null IIF.", NULL);
        AMP_DEBUG_EXIT("iif_register_node","-> %d", 0);
    	return 0;
    }

    memset((char*)iif, 0, sizeof(iif_t));
    iif->local_eid = eid;

    if(bp_attach() < 0)
    {
        AMP_DEBUG_ERR("iif_register_node","Failed to attach.", NULL);
        AMP_DEBUG_EXIT("iif_register_node","-> %d", 0);
        return 0;
    }
    
    if(bp_open((char *)eid.name, &(iif->sap)) < 0)
    {
        AMP_DEBUG_ERR("iif_register_node","Failed to open %s.", eid.name);
        AMP_DEBUG_EXIT("iif_register_node","-> %d", 0);
        return 0;
    }

    AMP_DEBUG_INFO("iif_register_node","Registered Agent as %s.", eid.name);

    
    AMP_DEBUG_EXIT("iif_register_node","-> %d", 1);
    return 1;
}




int
msg_bp_send(const blob_t *data, void *ctx)
{
  iif_t *iif = ctx;

  Sdr sdr = bp_get_sdr();
  if (sdr_begin_xn(sdr) < 0)
  {
    AMP_DEBUG_ERR("iif_send","Unable to start transaction.", NULL);
    return AMP_FAIL;
  }

  Object extent = sdr_malloc(sdr, data->length);
  if(extent)
  {
    sdr_write(sdr, extent, (char *) data->value, data->length);
  }
  else
  {
    AMP_DEBUG_ERR("iif_send","Can't write to NULL extent.", NULL);
    sdr_cancel_xn(sdr);
    return AMP_FAIL;
  }
  if (sdr_end_xn(sdr) < 0)
  {
      AMP_DEBUG_ERR("iif_send","Can't close transaction?", NULL);
  }

  Object content = ionCreateZco(ZcoSdrSource, extent, 0, data->length, BP_STD_PRIORITY, 0, ZcoOutbound, NULL);
  if(content == 0 || content == (Object) ERROR)
  {
      AMP_DEBUG_ERR("iif_send","Zero-Copy Object creation failed.", NULL);
      AMP_DEBUG_EXIT("iif_send", "->0.", NULL);
      return 0;
  }

  Object newBundle = 0;
  int res = bp_send(
    iif->sap,
    iif->peer_eid.name,       // recipient
    NULL,                   // report-to
    300,                    // lifespan (?)
    BP_STD_PRIORITY,        // Class-of-Service / Priority
    NoCustodyRequested,     // Custody Switch
    0,                      // SRR Flags
    0,                      // ACK Requested
    NULL,                   // Extended COS
    content,                // ADU
    &newBundle              // New Bundle
  );
  if (res != 1)
  {
    AMP_DEBUG_ERR("iif_send","Send failed (%d) to %s", res, iif->peer_eid.name);
    AMP_DEBUG_EXIT("iif_send", "->0.", NULL);
    return AMP_FAIL;
  }
  return AMP_OK;
}

blob_t *
msg_bp_recv(msg_metadata_t *meta, int *success, void *ctx)
{
  iif_t *iif = ctx;

  Sdr sdr = bp_get_sdr();
  int res;

  *success = AMP_FAIL;

  BpDelivery dlv;
  memset(&dlv, 0, sizeof(BpDelivery));

  while (true)
  {
    // Timeout is required to check agent running status
    static const int timeout = 5;
    if((res = bp_receive(iif->sap, &dlv, timeout)) < 0)
    {
      AMP_DEBUG_INFO("iif_receive","bp_receive failed. Result: %d.", res);
      *success = AMP_SYSERR;
      return NULL;
    }
    switch(dlv.result)
    {
      case BpEndpointStopped:
        /* The endpoint stopped? Panic.*/
        AMP_DEBUG_INFO("iif_receive","Endpoint stopped.");
        return NULL;

      case BpPayloadPresent:
        /* Clear to process the payload. */
        AMP_DEBUG_INFO("iif_receive", "Payload present.");
        break;

      default:
        /* No message yet. */
        continue;
    }
  }
  int content_len = zco_source_data_length(sdr, dlv.adu);

  blob_t *data;
  if((data = blob_create(NULL, 0, content_len)) == NULL)
  {
      *success = AMP_SYSERR;

      AMP_DEBUG_ERR("iif_receive","Can't alloc %d of msg.", content_len);
      bp_release_delivery(&dlv, 1);
      return NULL;
  }

  /* Step 3: Read the bundle in from the ZCO. */
  if (sdr_begin_xn(sdr) < 0)
  {
      *success = AMP_SYSERR;

      blob_release(data, 1);
      AMP_DEBUG_ERR("iif_receive","Can't start transaction.", NULL);
      bp_release_delivery(&dlv, 1);
      return NULL;
  }

  ZcoReader reader;
  zco_start_receiving(dlv.adu, &reader);
  data->length = zco_receive_source(sdr, &reader, data->alloc, (char*)data->value);

  if((sdr_end_xn(sdr) < 0) || (data->length == 0))
  {
      *success = AMP_SYSERR;

      AMP_DEBUG_ERR("iif_receive", "Unable to process received bundle.", NULL);
      blob_release(data, 1);
      bp_release_delivery(&dlv, 1);
      return NULL;
  }

  istrcpy(meta->senderEid.name, dlv.bundleSourceEid,
                  sizeof meta->senderEid.name);
  istrcpy(meta->originatorEid.name, dlv.bundleSourceEid,
                  sizeof meta->originatorEid.name);
  istrcpy(meta->recipientEid.name, iif->local_eid.name,
                  sizeof meta->recipientEid.name);
  bp_release_delivery(&dlv, 1);

  *success = AMP_OK;
  return data;
}
