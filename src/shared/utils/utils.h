/******************************************************************************
 **                           COPYRIGHT NOTICE
 **      (c) 2012 The Johns Hopkins University Applied Physics Laboratory
 **                         All rights reserved.
 ******************************************************************************/

/*****************************************************************************
 **
 ** File Name: utils.h
 **
 ** Description: This file contains the definitions, prototypes, constants, and
 **              other information for general utilities in the AMP system.
 **
 ** Notes:
 **
 ** Assumptions:
 **
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR         DESCRIPTION
 **  --------  ------------   ---------------------------------------------
 **  10/29/12  E. Birrane     Initial Implementation (JHU/APL)
 **  ??/??/16  E. Birrane     Added Serialize/Deserialize Functions.
 **                           Added "safe" memory functions.
 **                           Document Updates (Secure DTN - NASA: NNX14CS58P)
 **  09/02/18  E. Birrane     Removed Serialize/Deserialize functions (JHU/APL)
 *****************************************************************************/

#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>
#include <osapi-clock.h>
#include "shared/platform.h"
#include "shared/primitives/blob.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * +--------------------------------------------------------------------------+
 * |							  CONSTANTS  								  +
 * +--------------------------------------------------------------------------+
 */

// Fatal system error. Typically resource exhausted or underlying system error
#define AMP_SYSERR (-1)
// Non-fatal failure to do something.
#define AMP_FAIL (0)
// Successfully did something.
#define AMP_OK (1)


#ifndef MIN
#define MIN(x,y) (x < y ? x : y)
#endif

#define SUCCESS 1
#define FAILURE 0

/*
 * +--------------------------------------------------------------------------+
 * |							  	MACROS  								  +
 * +--------------------------------------------------------------------------+
 */
#define STAKE(size) utils_safe_take(size)
#define SRELEASE(ptr) utils_safe_release(ptr)

/*
 * +--------------------------------------------------------------------------+
 * |							  DATA TYPES  								  +
 * +--------------------------------------------------------------------------+
 */

/*
 * +--------------------------------------------------------------------------+
 * |						  FUNCTION PROTOTYPES  							  +
 * +--------------------------------------------------------------------------+
 */


int8_t utils_mem_int();
void   utils_mem_teardown();
void   utils_safe_release(void* ptr);
void*  utils_safe_take(size_t size);

//unsigned long utils_atox(char *s);
unsigned long utils_atox(const char *s, int *success);

char*    utils_hex_to_string(const uint8_t *buffer, size_t size);
void     utils_print_hex(const unsigned char *s, size_t len);

blob_t*  utils_string_to_hex(const char *value);


#ifdef __cplusplus
}
#endif

#endif  /* _UTILS_H_ */
