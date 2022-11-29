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
  APP_ST_ERROR,
  APP_ST_SLEEP,
  APP_ST_GET_BASE_STATUS_SEND,
  APP_ST_GET_BASE_STATUS_RECV,
  APP_ST_BASE_READY,
  APP_ST_TRIG_BASE_PROGRESS,
  APP_ST_TRIG_BASE_DONE,
};

void vdSetNewState(E_APP_ST eNewState);
bool bIsNewState(void);
void irqBut1WakeUp(void);

E_APP_ST eAppSt;
bool bNewSt;
uint8_t u8BaseStatus;

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
  vdSetNewState(APP_ST_INIT);
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
        eAppSt = APP_ST_ERROR;
      }
      break;
    }

    case APP_ST_ERROR: 
    {
      vdLed_FixRed();
      delay(2000);
      vdSetNewState(APP_ST_SLEEP);
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
        vdTimeoutSet(1000);
        while((!Serial) && (!bTimeoutExpired()));

        // Check RF driver is operational.
        if(bRfDrv_IsOperational()) {
          vdSetNewState(APP_ST_GET_BASE_STATUS_SEND);
        }
        else {
          vdSetNewState(APP_ST_ERROR);
        }
      }
      break;
    }

    case APP_ST_GET_BASE_STATUS_SEND:
    {
      // Send packet.
      vdRfDrv_SendPacket(U8_PKTVAL_GET_STATUS);
      vdSetNewState(APP_ST_GET_BASE_STATUS_RECV);

      break;
    }

    case APP_ST_GET_BASE_STATUS_RECV:
    {
      // Listen to response.
      vdLed_FixPurple();
      if( (bRfDrv_RecvPacketBlocking(2000, &u8BaseStatus)) && 
          (U8_PKTVAL_READY == u8BaseStatus) ) 
      {
        vdSetNewState(APP_ST_BASE_READY);
      }
      else {
        vdSetNewState(APP_ST_ERROR);    
      }
      break;
    }

    #define U32_BASE_RECV_TIMEOUT ((uint32_t)5000)
    case APP_ST_BASE_READY: 
    {
      if(bIsNewState()) {
        vdLed_FixGreen();
        vdTimeoutSet(10000);
      }
      else if(mBUT_IsTrigPressed()) {
        vdSetNewState(APP_ST_TRIG_BASE_PROGRESS);
      }
      else if(mBUT_IsArmdPressed()) {
        vdSetNewState(APP_ST_GET_BASE_STATUS_SEND);
      }
      else if(bTimeoutExpired())
      {
        vdSetNewState(APP_ST_SLEEP);
      }
      break;
    }

    case APP_ST_TRIG_BASE_PROGRESS: {
      static bool bTrigDone;
      static uint8_t u8NbNoResponses;
      static bool bNoResponse;

      if(bIsNewState()) {
        bTrigDone = false;
        u8NbNoResponses = 0;
        vdLed_FixOrange();
      }

      bNoResponse = false;
      vdRfDrv_SendPacket(bTrigDone ? U8_PKTVAL_GET_STATUS : U8_PKTVAL_TRIGGER);
      bTrigDone = true;
      if(bRfDrv_RecvPacketBlocking(500, &u8BaseStatus)) 
      {
        if(U8_PKTVAL_PROGRESS == u8BaseStatus) {
          u8NbNoResponses = 0;
          delay(500);
        }
        else if(U8_PKTVAL_DONE) {
          vdSetNewState(APP_ST_TRIG_BASE_DONE);
        }
        else if(U8_PKTVAL_ERROR) {
          vdSetNewState(APP_ST_ERROR); // Clear error response of the base.
        }
        else{ // Unknown response.
          u8NbNoResponses++;
          bNoResponse = true;
        }
      }
      else {
        
        bNoResponse = true;
      }

      if(bNoResponse) {
        if(++u8NbNoResponses >= 5) {
          vdSetNewState(APP_ST_ERROR);
        }
        else {
          delay(500);
        }
      }
      break;
    }

    case APP_ST_TRIG_BASE_DONE: 
    {
      vdLed_FixGreen();
      delay(2000);
      vdSetNewState(APP_ST_SLEEP);
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

