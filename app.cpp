#include "app.h"

#include "rf_drv.h"
#include "led.h"
#include "timeout.h"
#include "actuator.h"

enum E_APP_ST {
  APP_ST_INIT,
  APP_ST_ERROR,
  APP_ST_REWIND,
  APP_ST_READY,
  APP_ST_TRIGGER,
  APP_ST_DONE,
};

void vdSetNewState(E_APP_ST eNewState);
bool bIsNewState(void);

E_APP_ST eAppSt;
bool bNewSt;

// Requires that the RF driver is initialized.
void vdApp_Init(void)
{
  Serial.print(F("Base ready"));
  Serial.println();
}

void vdApp_Task(void)
{
  switch(eAppSt)
  {
    default:
    case APP_ST_INIT: 
    {
      if(bRfDrv_IsOperational()) {
        eAppSt = APP_ST_REWIND;
      }
      else {
        eAppSt = APP_ST_ERROR;
      }
      break;
    }

    case APP_ST_ERROR: 
    {
      if(bIsNewState()){
        vdLed_FixRed();
      }
      else if(bRfDrv_RecvStatusReqBlocking(10000)) {
        vdRfDrv_SendPacket(U8_PKTVAL_ERROR);
      }
      break;
    }

    case APP_ST_REWIND: 
    {
      if(bIsNewState()){
        vdLed_FixPurple();
        vdTimeoutSet(30000);
        vdActuator_Rewind();
      }
      else if(bTimeoutExpired()) {
        vdSetNewState(APP_ST_READY);
        vdActuator_Stop();
      }
      else if(bRfDrv_RecvStatusReqBlocking(u32TimeoutGetLeftTime() + 1)) {
        vdRfDrv_SendPacket(U8_PKTVAL_PROGRESS);
      }
      break;
    }

    case APP_ST_READY: 
    {
      if(bIsNewState()){
        vdLed_FixGreen();
      }
      else if(bRfDrv_RecvStatusReqBlocking(10000)) {
        vdRfDrv_SendPacket(U8_PKTVAL_READY);
      }
      break;
    }

  } //end switch
}

// ============== PRIVATE ===============
void vdSetNewState(E_APP_ST eNewState)
{
  bNewSt = true;
  eAppSt = eNewState;
}

bool bIsNewState(void)
{
  if(bNewSt)
  {
    bNewSt = false;
    return true;
  }
  return false;
}

void irqBut1WakeUp(void) {
  // This function will be called once on device wakeup
  // You can do some little operations here (like changing variables which will be used in the loop)
  // Remember to avoid calling delay() and long running functions since this functions executes in interrupt context
}

