#ifndef __TIMEOUT_H__
#define __TIMEOUT_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void vdTimeoutSet(uint32_t u32Timeout_ms);
bool bTimeoutExpired(void);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __TIMEOUT_H__