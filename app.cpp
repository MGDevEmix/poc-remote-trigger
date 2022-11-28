#include "delay.h"
#include "app.h"

#include <ArduinoLowPower.h>

#include "rf_drv.h"
#include "led.h"
#include "timeout.h"

#define BUT_ARMD 12
#define BUT_TRIG 11
#define mBUT_IsArmdPressed() (digitalRead(BUT_ARMD) == 0)
#define mBUT_IsTrigPressed() (digitalRead(BUT_TRIG) == 0)

enum E_APP_ST {
  APP_ST_INIT,
  APP_ST_INTERNAL_ERROR,
  APP_ST_SLEEP,
  APP_ST_GET_BASE_STATUS_SEND,
  APP_ST_GET_BASE_STATUS_RECV,
  APP_ST_BASE_NOT_READY_OR_NOT_FOUND,
  APP_ST_BASE_READY,
  APP_ST_TRIG_BASE_SEND,
  APP_ST_TRIG_BASE_RECV,
  APP_ST_TRIG_BASE_DONE,
  APP_ST_TRIG_BASE_ERROR
};

void vdSetNewState(E_APP_ST eNewState);
bool bIsNewState(void);
void irqBut1WakeUp(void);
/*
void tx_send_packet();
void tx_packet_is_OK();
void tx_packet_is_Error();
void rx_recv_packet();
void rx_packet_is_OK();
void rx_packet_is_Error();
void printElapsedTime();
*/
E_APP_ST eAppSt;
bool bNewSt;

// Requires that the RF driver is initialized.
void vdApp_Init(void)
{
  // Set BUTs as INPUT_PULLUP to avoid spurious wakeup
  // Attach a wakeup interrupt on pin 8, calling repetitionsIncrease when the device is woken up
  pinMode(BUT_ARMD, INPUT_PULLUP);
  pinMode(BUT_TRIG, INPUT_PULLUP);
  LowPower.attachInterruptWakeup(BUT_ARMD, irqBut1WakeUp, FALLING);

  Serial.print(F("Controller ready"));
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
        eAppSt = APP_ST_GET_BASE_STATUS_SEND;
      }
      else {
        eAppSt = APP_ST_INTERNAL_ERROR;
      }
      break;
    }

    case APP_ST_INTERNAL_ERROR:
    {
      if(bIsNewState()) {
        vdLed_FixRed();
        vdTimeoutSet(2000);
      }
      if(bTimeoutExpired()) {
        vdSetNewState(APP_ST_SLEEP);
      }
      break;
    }

    case APP_ST_SLEEP:
    {
      if(bIsNewState())
      {
        // Prepare sleep.
        vdLed_Off();
        Serial.end();  
        USBDevice.detach();  

        // Triggers an infinite sleep (the device will be woken up only by the registered wakeup sources)
        // The power consumption of the chip will drop consistently
        LowPower.sleep();

        //  Wakeup from BUT1, exit sleep.
        vdLed_FixBlue();
        USBDevice.attach(); 
        Serial.begin(9600);
        vdTimeoutSet(2000);
        while((!Serial) && (!bTimeoutExpired()));

        // Check RF driver is operational.
        if(bRfDrv_IsOperational()) {
          vdSetNewState(APP_ST_GET_BASE_STATUS_SEND);
        }
        else {
          vdSetNewState(APP_ST_INTERNAL_ERROR);
        }
      }
      break;
    }

    case APP_ST_GET_BASE_STATUS_SEND:
    {
      // Send packet.
      vdRfDrv_SendPacket();
      vdSetNewState(APP_ST_GET_BASE_STATUS_RECV);

      break;
    }

    case APP_ST_GET_BASE_STATUS_RECV:
    {
      uint8_t au8Packet;

      // Listen to response.
      vdLed_FixPurple();
      vdRfDrv_RecvPacket(2000, &u8Packet); // blocking.

      if(u8Packet == 1) { // Ready
        vdSetNewState(APP_ST_BASE_READY);
      } 
      else
      {
        vdSetNewState(APP_ST_BASE_NOT_READY_OR_NOT_FOUND);
      }
      break;
    }

    case APP_ST_BASE_NOT_READY_OR_NOT_FOUND: 
    {
      if(bIsNewState()) {
        vdLed_FixRed();
        delay(2000);
        vdSetNewState(APP_ST_SLEEP);
      }
      break;
    }

    case APP_ST_BASE_READY: 
    {
      if(bIsNewState()) {
        vdLed_FixGreen();
        vdTimeoutSet(10000);
      }
      else if(mBUT_IsTrigPressed()) {
        vdRfDrv_SendPacket();
        vdSetNewState(APP_ST_TRIG_BASE_RECV);
      }
      else if(bTimeoutExpired())
      {
        vdSetNewState(APP_ST_SLEEP);
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

