#ifndef _ASSERT_H
#define _ASSERT_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void kassert(char* file, int line);

#ifdef NDEBUG
#define assert( ignore ) ( (void) 0 )
#else
#define assert(expression)((expression)?(void) 0: kassert(__FILE__,  __LINE__))
#endif

#ifdef __cplusplus
}
#endif

#endif
