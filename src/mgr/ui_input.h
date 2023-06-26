/******************************************************************************
 **                           COPYRIGHT NOTICE
 **      (c) 2018 The Johns Hopkins University Applied Physics Laboratory
 **                         All rights reserved.
 ******************************************************************************/
/*****************************************************************************
 **
 ** \file ui_input.h
 **
 **
 ** Description: Functions to retrieve information from the user via a
 **              text-based interface.
 **
 **
 ** Notes:
 **
 ** Assumptions:
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR         DESCRIPTION
 **  --------  ------------   ---------------------------------------------
 **  05/24/15  E. Birrane     Initial Implementation (Secure DTN - NASA: NNX14CS58P)
 **  10/06/18  E. Birrane     Update to AMP v0.5 (JHU/APL)
 *****************************************************************************/

#ifndef _UI_INPUT_H
#define _UI_INPUT_H

#include "../shared/utils/nm_types.h"
#include "../shared/adm/adm.h"
#include "../shared/primitives/blob.h"
#include "../shared/primitives/rules.h"

#include "nm_mgr_ui.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_INPUT_BYTES 1024

#define TYPE_AS_MASK(type) (((amp_uvast)1) << ((amp_uvast)type))
#define TYPE_MATCHES_MASK(type, mask) (TYPE_AS_MASK(type) & mask)

#if (LONG_LONG_OKAY)
#define TYPE_MASK_ALL (0xFFFFFFFFFFFFFFFF)
#else
#define TYPE_MASK_ALL (0xFFFFFFFF)
#endif

int ui_input_get_line(const char *prompt, char *line, int max_len);

/*
 * AMM Object Input Functions
 */

uint8_t ui_input_adm_id();

/*
 * User input methods for basic data types.
 */
uint8_t  ui_input_byte(const char *prompt);
int32_t  ui_input_int(const char *prompt);
float    ui_input_real32(const char *prompt);
double   ui_input_real64(const char *prompt);
char *   ui_input_string(const char *prompt);
uint32_t ui_input_uint(const char *prompt);
amp_uvast    ui_input_uvast(const char *prompt);
amp_vast     ui_input_vast(const char *prompt);

/*
 * User input for compound object types.
 */

ac_t*   ui_input_ac(const char *prompt);

ari_t*  ui_input_ari(const char *prompt, uint8_t adm_id, amp_uvast mask);
ari_t*  ui_input_ari_build(amp_uvast mask);
int     ui_input_ari_flags(uint8_t *flag);
ari_t*  ui_input_ari_list(uint8_t adm_id, amp_uvast mask);
ari_t*  ui_input_ari_lit(const char *prompt);
ari_t*  ui_input_ari_raw(uint8_t no_file);
int     ui_input_ari_type(amp_uvast mask);
int     ui_input_parms(ari_t *id);

tnv_t*  ui_input_tnv(int type, char *prompt);
tnvc_t* ui_input_tnvc(const char *prompt);


/* Input for helper types. */
blob_t*  ui_input_blob(const char *prompt, uint8_t no_file);
blob_t*  ui_input_file_contents(const char *prompt);


ctrl_t* ui_input_ctrl(const char *prompt);
expr_t* ui_input_expr(const char *prompt);
op_t* ui_input_oper(const char *prompt);
rpt_t* ui_input_rpt(const char *prompt);
rpttpl_t* ui_input_rpttpl(const char *prompt);
rule_t *ui_input_rule(const char *prompt);

tbl_t* ui_input_tbl(const char *prompt);
tblt_t* ui_input_tblt(const char *prompt);

rule_t *ui_input_tbr(const char *prompt);

var_t* ui_input_var(const char *prompt);

tnvc_t* ui_input_tnvc(const char *prompt);

macdef_t *ui_input_mac(const char *prompt);


#ifdef __cplusplus
}
#endif

#endif  /* _UI_INPUT_H_ */
