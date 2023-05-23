/******************************************************************************
 **                           COPYRIGHT NOTICE
 **      (c) 2012 The Johns Hopkins University Applied Physics Laboratory
 **                         All rights reserved.
 ******************************************************************************/

/*****************************************************************************
 **
 ** \file utils.c
 **
 ** Subsystem:
 **          Network Management Utilities
 **
 ** Description:
 **
 ** Notes:
 **
 ** Assumptions:
 **
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR         DESCRIPTION
 **  --------  ------------   ---------------------------------------------
 **  10/29/12  E. Birrane     Initial Implementation. (JHU/APL)
 **  ??/??/16  E. Birrane     Added Serialize/Deserialize Functions.
 **                           Added "safe" memory functions.
 **                           Document Updates (Secure DTN - NASA: NNX14CS58P)
 **  07/04/16  E. Birrane     Added limited support for serialize/deserialize
 **                           floats and doubles. (Secure DTN - NASA: NNX14CS58P)
 **  09/02/18  E. Birrane     Removed Serialize/Deserialize functions (JHU/APL)
 *****************************************************************************/

#include <stdlib.h>
#include <stdarg.h>
#include <osapi-mutex.h>
#include <osapi-error.h>
#include "shared/platform.h"
#include "shared/utils/debug.h"
#include "shared/utils/utils.h"
#include "vector.h"

static osal_id_t gMemMutex;


#if AMP_DEBUGGING == 1
void amp_log(int level, char label, const char *file, int line, const char *func, const char *fmt, ...)
{
  if (level < AMP_DEBUG_LVL)
  {
    return;
  }

  va_list valist;
  char gAmpMsg[AMP_GMSG_BUFLEN];
  va_start(valist, fmt);
  vsnprintf(gAmpMsg, AMP_GMSG_BUFLEN, (char *) fmt, valist);
  va_end(valist);
  fprintf(stderr, "[%s:%d] %c %s %s\n", file, line, label, func, gAmpMsg);
}

#endif

int8_t utils_mem_int()
{
	if(OS_MutSemCreate(&gMemMutex, "utils", 0) != OS_SUCCESS)
	{
		AMP_DEBUG_ERR("utils_mem_int", "Cannot allocate memory mutex.");
		return AMP_SYSERR;

	}
	return AMP_OK;
}

void utils_mem_teardown()
{
	OS_MutSemDelete(gMemMutex);
}


void* utils_safe_take(size_t size)
{
	void *result;

	OS_MutSemTake(gMemMutex);
#ifndef USE_MALLOC
	result = MTAKE(size);
#else
	result = malloc(size); /* Use this when memory debugging with valgrind. */
#endif
	if(result != NULL)
	{
		memset(result,0,size);
	}
	OS_MutSemGive(gMemMutex);
	return result;
}

void utils_safe_release(void* ptr)
{
	if(ptr == NULL)
	{
		return;
	}

	OS_MutSemTake(gMemMutex);
#ifndef USE_MALLOC
	MRELEASE(ptr);
#else
	free(ptr); /* Use this when memory debugging with valgrind. */
#endif
	OS_MutSemGive(gMemMutex);
}

/******************************************************************************
 *
 * \par Function Name: atox
 *
 * \par Initializes an unsigned long with a value from a string. For example,
 * 		return an unsigned long with the value 0x01020304 from the string
 * 		"01020304".
 *
 * \param[in]  s        The string to be converted to a hex array.
 * \param[out] success  Whether the conversion was a success.
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  11/25/12  E. Birrane     Initial implementation. (JHU/APL)
 *  12/16/12  E. Birrane     Added success return, error checks, logging (JHU/APL)
 *  09/02/18  E. Birrane     Updated to not hard-code success values. (JHU/APL)
 *****************************************************************************/

unsigned long utils_atox(const char *s, int *success)
{
	unsigned long result = 0;
	int i = 0;
	int mult = 0;
	int j = 0;
	int temp = 0;

	AMP_DEBUG_ENTRY("utils_atox","(%p, %p)", s, success);

	/* Step 0 - Sanity Check. */
	if (success == NULL)
	{
		AMP_DEBUG_ERR("utils_atox","Bad Args.");
		AMP_DEBUG_ENTRY("utils_atox","->0.");
		return AMP_FAIL;
	}

	*success = AMP_FAIL;
	if(s == NULL)
	{
		AMP_DEBUG_ERR("utils_atox","Bad Args.");
		AMP_DEBUG_ENTRY("utils_atox","->0.");
		return AMP_FAIL;
	}

	/*
	 * Step 0.5 Handle case where string starts with "0x" by simply
	 * advancing s to skip over it. This won't modify s from the
	 * caller point of view.
	 */
	if((s[0]=='0') && (s[1]=='x'))
	{
		s = s + 2;
	}

	*success = AMP_OK;

	/* Step 1 - Make sure string isn't too long. Since every character in the
	 *          string represents a nibble, 2 characters are a byte, making
	 *          the longest valid length sizeof(unsigned long) * 2.
	 */
	if(strlen(s) > (sizeof(unsigned long) * 2))
	{
		AMP_DEBUG_ERR("utils_atox","x UI: String %s too long to convert to hex unsigned long.", s);
		*success = AMP_FAIL;
		AMP_DEBUG_ENTRY("utils_atox","->0.");
		return AMP_FAIL;
	}

	/* Step 2 - Walk through the string building the result. */
	for(i = strlen(s)-1; i >= 0; i--)
	{
		mult = j * 16;
		if(mult == 0)
		{
			mult = 1;
		}

		switch(s[i])
		{
		case '0': case '1': case '2': case '3': case '4': case '5': case '6':
		case '7': case '8': case '9':
				  temp = s[i] - '0';
				  result += temp * mult;
				  break;
		case 'A': case 'a': result += 10 * mult; break;
		case 'B': case 'b': result += 11 * mult; break;
		case 'C': case 'c': result += 12 * mult; break;
		case 'D': case 'd': result += 13 * mult; break;
		case 'E': case 'e': result += 14 * mult; break;
		case 'F': case 'f': result += 15 * mult; break;
		case '\n': break; // Ignore newline characters (ie: copy/paste line-wrapping errors)
		default:
			AMP_DEBUG_ERR("utils_atox","x Non-hex character: %c", s[i]);
			*success = AMP_FAIL;
			j--;
			break;
		}
		j++;
	}

//	AMP_DEBUG_INFO("utils_atox","i UI: Turned string %s to number %x.", s, result);
	return result;
}




/******************************************************************************
 *
 * \par Function Name: utils_hex_to_string
 *
 * \par Purpose: Constructs a character string of values from a buffer.
 *
 * \return NULL - Failure.
 * 		   !NULL - Desired string
 *
 * \param[in]  buffer  The buffer whose string representation is desired.
 * \param[in]  size    Size of the buffer, in bytes,
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  10/14/12  E. Birrane     Initial implementation (JHU/APL)
 *****************************************************************************/

char *utils_hex_to_string(const uint8_t *buffer, size_t size)
{
    char *result = NULL;
    uint32_t char_size = 0;

    char temp[3];
    size_t i = 0;
    int r = 0;

    AMP_DEBUG_ENTRY("utils_hex_to_string","(%x,%d)",
    		          (size_t) buffer, size);

    /* Each byte requires 2 characters to represent in HEX. Also, require
     * three additional bytes to capture '0x' and NULL terminator.
     */
    char_size = (2 * size) + 3;
    result = (char *) STAKE(char_size);

    if(result == NULL)
    {
        AMP_DEBUG_ERR("utils_hex_to_string", "Cannot allocate %d bytes.",
        		        char_size);
        AMP_DEBUG_EXIT("utils_hex_to_string", "-> NULL.", NULL);
        return NULL;
    }

    result[0] = '0';
    result[1] = 'x';
    r = 2;

    for(i = 0; i < size; i++)
    {
        sprintf(temp, "%.2x", (unsigned int)buffer[i]);
        result[r++] = temp[0];
        result[r++] = temp[1];
    }

    result[r] = '\0';

    AMP_DEBUG_EXIT("mid_to_string","->%s.", result);

    return result;
}



/******************************************************************************
 *
 * \par Function Name: utils_print_hex
 *
 * \par Purpose: Prints a string as a series of hex characters.
 *
 * \param[in]  s     The string to be printed.
 * \param[in]  len   The length of the string.
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  10/14/12  E. Birrane     Initial implementation (JHU/APL)
 *****************************************************************************/

void utils_print_hex(const unsigned char *s, size_t len)
{
	size_t i;

	printf("0x");
	for(i = 0; i < len; i++)
	{
		printf("%.2x", s[i]);
	}
	printf("\n");
}



/******************************************************************************
 *
 * \par Function Name: utils_string_to_hex
 *
 * \par Purpose: Converts an ASCII string representing hex values to a byte
 *               array of those hex values.
 *
 * \return NULL - Failure.
 * 		   !NULL - The desired byte array.
 *
 * \param[in]   value  The string to be converted to hex.
 * \param[out]  size   The length of the converted byte array.
 *
 * \par Notes:
 *
 * Modification History:
 *  MM/DD/YY  AUTHOR         DESCRIPTION
 *  --------  ------------   ---------------------------------------------
 *  10/14/12  E. Birrane     Initial implementation (JHU/APL)
 *****************************************************************************/

uint8_t getNibble(char c)
{
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'a' && c <= 'f') return c - 'a' + 10;
	if( c>= 'A' && c <= 'F') return c - 'A' + 10;

	return 255;
}



blob_t* utils_string_to_hex(const char *value)
{
	blob_t *result = NULL;
	char tmp_s[3];
	int len = 0;
	int success = 0;
	int pad = 0; 
	size_t size = 0;

	/*
	 * Step 0.5 Handle case where string starts with "0x" by simply
	 * advancing s to skip over it. This won't modify s from the
	 * caller point of view.
	 */
	if((value[0]=='0') && (value[1]=='x'))
	{
		value = value + 2;
	}


	/* Step 1 - Figure out the size of the byte array. Since each ASCII
	 *          character represents a nibble, the size of the byte array is
	 *          half the size of the string (accounting for odd values).
	 */
	len = strlen((char*)value);

	if((len%2) == 0)
	{
	  size = len/2;
	}
	else
	{
		size = (len/2) + 1;
       pad = 1;
	}

	if((result = blob_create(NULL, 0, size+1)) == NULL)
	{
		AMP_DEBUG_ERR("utils_string_to_hex","Can't Alloc %d bytes.", size);
		AMP_DEBUG_EXIT("utils_string_to_hex", "->NULL.", NULL);
		return NULL;
	}

	/* Step 2 - For each byte, copy in the nibbles. */
	tmp_s[2] = '\0';
	int incr = 1;
    int i = 0;

	for(i = 0; i < len;)
	{
		if(pad == 1)
		{
			tmp_s[0] = '0';
			tmp_s[1] = value[i];
			pad = 0;
			incr = 1;
		}
		else
		{
			memcpy(tmp_s, &(value[i]), 2);
			incr = 2;
		}
		uint8_t tmp_x = (uint8_t) utils_atox(tmp_s, &success);
		blob_append(result, &tmp_x, 1);
	
		i += incr;
		if(success == 0)
		{
			AMP_DEBUG_ERR("utils_string_to_hex","Can't AtoX %s.", tmp_s);
			blob_release(result, 1);

			AMP_DEBUG_EXIT("utils_string_to_hex", "->NULL.", NULL);
			return NULL;
		}
	}

	AMP_DEBUG_EXIT("utils_string_to_hex", "->%#llx.", result);
	return result;
}

int utils_time_delta(OS_time_t *result, const OS_time_t *t1, const OS_time_t *t2)
{
  *result = OS_TimeSubtract(*t1, *t2);
  return result->ticks < 0;
}

/* Return number of micro-seconds that have elapsed since the passed-in time.*/
vast    utils_time_cur_delta(const OS_time_t *t1)
{
	OS_time_t cur;
	OS_time_t delta;

        if (OS_GetLocalTime(&cur) != OS_SUCCESS)
        {
          return 0;
        }
          
	utils_time_delta(&delta, &cur, t1);
        return OS_TimeGetTotalMicroseconds(delta);
}






