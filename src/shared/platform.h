#ifndef PLATFORM_H_
#define PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void * uaddr;
//typedef uint32_t uint;
//typedef int32_t int;
typedef uint64_t uvast;
typedef int64_t vast;


#define CHKERR(expr) if (!(expr)) { return 1; }
#define CHKVOID(expr) if (!(expr)) { return; }
#define CHKZERO(expr) if (!(expr)) { return 0; }
#define CHKNULL(expr) if (!(expr)) { return NULL; }
#define CHKUSR(expr,usr) if (!(expr)) { return usr; }

#ifdef __cplusplus
}
#endif

#endif  /* PLATFORM_H_ */
