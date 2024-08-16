
#ifndef CACE_CASESTR_H_
#define CACE_CASESTR_H_

#include <strings.h>

/// Case-insensitive equality comparison
#define M_CASESTR_EQUAL(a, b) (strcasecmp((a), (b)) == 0)

/** M*LIB OPLIST for case-insenstive C-string values.
 * This is intended to be used as dict/tree keys of type "const char *" with
 * external memory management.
 */
#define M_CASESTR_OPLIST M_OPEXTEND(M_CSTR_OPLIST, EQUAL(M_CASESTR_EQUAL), CMP(strcasecmp))

#endif /* CACE_CASESTR_H_ */
