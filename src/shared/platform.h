#ifndef NM_SHARED_PLATFORM_H_
#define NM_SHARED_PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void * amp_uaddr;
typedef uint32_t amp_uint;
typedef int32_t amp_int;
typedef uint64_t amp_uvast;
typedef int64_t amp_vast;

#ifndef _PLATFORM_H_
#define CHKERR(expr) if (!(expr)) { return 1; }
#define CHKVOID(expr) if (!(expr)) { return; }
#define CHKZERO(expr) if (!(expr)) { return 0; }
#define CHKNULL(expr) if (!(expr)) { return NULL; }
#endif
#define CHKUSR(expr,usr) if (!(expr)) { return usr; }

#ifdef __cplusplus
}
#endif

#endif  /* NM_SHARED_PLATFORM_H_ */
