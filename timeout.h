#ifndef __TIMEOUT_H__
#define __TIMEOUT_H__

#include <Arduino.h>

#ifdef __cplusplus
 extern "C" {
#endif

void vdTimeoutSet(uint32_t u32Timeout_ms);
void vdTimeoutReload(void);
bool bTimeoutExpired(void);
uint32_t u32TimeoutGetLeftTime(void);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __TIMEOUT_H__