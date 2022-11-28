#ifndef __RF_DRV_H__
#define __RF_DRV_H__

#include <Arduino.h>

#ifdef __cplusplus
 extern "C" {
#endif

void vdRfDrv_Init(void);
bool bRfDrv_IsOperational(void);
void vdRfDrv_SendPacket(void);
void vdRfDrv_RecvPacket(uint32_t u32Timeout_ms, uint8_t*);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __RF_DRV_H__