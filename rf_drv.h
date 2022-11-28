#ifndef __RF_DRV_H__
#define __RF_DRV_H__

#include <Arduino.h>

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum {
  RF_PROFILE_BASE,
  RF_PROFILE_CONTROLLER,
} E_RF_PROFILE;

// Value of the packet data.
#define U8_PKTVAL_BASE       ((uint8_t)0xB0)
#define U8_PKTVAL_CTRL       ((uint8_t)0xC0)
#define U8_PKTVAL_GET_STATUS ((uint8_t)0x10)
#define U8_PKTVAL_TRIGGER    ((uint8_t)0x11)
#define U8_PKTVAL_READY      ((uint8_t)0x20)
#define U8_PKTVAL_DONE       ((uint8_t)0x21)
#define U8_PKTVAL_ERROR      ((uint8_t)0x30)
#define U8_PKTVAL_PROGRESS   ((uint8_t)0x31)

void vdRfDrv_Init(E_RF_PROFILE);
bool bRfDrv_IsOperational(void);
void vdRfDrv_SendPacket(uint8_t);
bool bRfDrv_RecvPacketBlocking(uint32_t u32Timeout_ms, uint8_t* pu8Data);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __RF_DRV_H__