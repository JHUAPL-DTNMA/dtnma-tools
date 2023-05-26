/******************************************************************************
 **                           COPYRIGHT NOTICE
 **      (c) 2011 The Johns Hopkins University Applied Physics Laboratory
 **                         All rights reserved.
 ******************************************************************************/
/*****************************************************************************
 ** \file nm_mgr.h
 **
 ** File Name: nm_mgr.h
 **
 ** Subsystem:
 **          Network Manager Application
 **
 ** Description: This file implements the DTNMP Manager user interface
 **
 ** Notes:
 **
 ** Assumptions:
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR          DESCRIPTION
 **  --------  ------------    ---------------------------------------------
 **  09/01/11  V. Ramachandran Initial Implementation (JHU/APL)
 **  08/19/13  E. Birrane      Documentation clean up and code review comments. (JHU/APL)
 **  08/21/16  E. Birrane      Update to AMP v02 (Secure DTN - NASA: NNX14CS58P)
 **  10/06/18   E. Birrane     Update to AMP v0.5 (JHU/APL)
 *****************************************************************************/

#ifndef NM_MGR_RX_H_
#define NM_MGR_RX_H_

#ifdef __cplusplus
extern "C" {
#endif

void*    mgr_rx_thread(void *arg);

#ifdef __cplusplus
}
#endif

#endif  /* NM_MGR_RX_H_ */
