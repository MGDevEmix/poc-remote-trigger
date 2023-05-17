#include "api/Common.h"
#include "app.h"

#include "rf_drv.h"
#include "led.h"
#include "timeout.h"
#include "actuator.h"
#include "pins.h"

#include <Wire.h>
#include "SparkFun_LIS2DH12.h"

#define U32_ACTUATOR_CMD_TIME ((uint32_t)3000)

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
uint8_t u8DataFromCtrl;

SPARKFUN_LIS2DH12 accel;       //Create instance

// Requires that the RF driver is initialized.
void vdApp_Init(void)
{
  Serial.print(F("Base ready"));
  Serial.println();
  Serial.setTimeout(100);
  vdSetNewState(APP_ST_INIT);
}

void vdApp_Task(void)
{
  static String str;
  if(Serial.available()) {
    str = Serial.readString();
    if(str.length() > 0) {
      if(str.equals("led on")) {
        digitalWrite(ONBOARD_LED, HIGH);
        Serial.print("\nled on ok");
      }
      else if(str.equals("led off")) {
        digitalWrite(ONBOARD_LED, LOW);
        Serial.print("\nled off ok");
      }
      else if(str.equals("vin meas")) {
        float f = analogRead(VIN_MES);
        f = f / 1024 * 3.3 * 43 / 10 + 0.12;
        str = "\nread " + String(f);
        Serial.print(str);
      }
      else if(str.startsWith("warning ")){
        str.remove(0, 8);
        if(str.startsWith("blue") && str.endsWith("on")) {
          digitalWrite(WARNING_BLUE, HIGH);
        }
        else if(str.startsWith("blue") && str.endsWith("off")) {
          digitalWrite(WARNING_BLUE, LOW);
        }
        else if(str.startsWith("red") && str.endsWith("on")) {
          digitalWrite(WARNING_RED, HIGH);
        }
        else if(str.startsWith("red") && str.endsWith("off")) {
          digitalWrite(WARNING_RED, LOW);
        }
        else if(str.startsWith("green") && str.endsWith("on")) {
          digitalWrite(WARNING_GREEN, HIGH);
        }
        else if(str.startsWith("green") && str.endsWith("off")) {
          digitalWrite(WARNING_GREEN, LOW);
        }
        else if(str.startsWith("blink") && str.endsWith("on")) {
          digitalWrite(WARNING_BLINK, HIGH);
        }
        else if(str.startsWith("blink") && str.endsWith("off")) {
          digitalWrite(WARNING_BLINK, LOW);
        }
        else if(str.startsWith("buzzer") && str.endsWith("on")) {
          digitalWrite(WARNING_BUZZER, HIGH);
        }
        else if(str.startsWith("buzzer") && str.endsWith("off")) {
          pinMode(WARNING_BUZZER, OUTPUT); //$TODO : init pins.
          digitalWrite(WARNING_BUZZER, LOW);
        }
      }
      else if(str.startsWith("accel ")){
        if(str.endsWith("init")) {
          Wire.begin();
          if(accel.begin()) {
            Serial.print("\nok");
          }
          else {
            Serial.print("\nERROR");
          }
        }
        else if(str.endsWith("data")) {
          if (accel.available())
          {
            Serial.print("\nAcc [mg]: ");
            Serial.print(accel.getX(), 1);
            Serial.print(" x, ");
            Serial.print(accel.getY(), 1);
            Serial.print(" y, ");
            Serial.print(accel.getZ(), 1);
            Serial.print(" z, ");
            Serial.print(accel.getTemperature(), 1);
            Serial.print("C");
          }
          else {
            Serial.print("\ndata not yet available");
          }
        }
        else if(str.endsWith("it cfg")) {
          //Configure the LIS2DH12 to pull pin low when it is tilted above a certain angle
          pinMode(ACCEL_INT, INPUT_PULLUP);
          accel.setDataRate(LIS2DH12_POWER_DOWN); //Stop measurements
          accel.setInt1(false); //INT1_CFG - enable X and Y events
          accel.setIntPolarity(LOW); //Set INT POLARITY to Active Low
          accel.setInt1IA1(true); //Set INT1 interrupt
          accel.setInt1Threshold(31); //45 degree tilt before interrupt
          accel.setInt1Duration(9);
          accel.setInt1Latch(false);
          while (accel.getInt1()) delay(10); //Reading int will clear it
          accel.setDataRate(LIS2DH12_ODR_400Hz); 
          accel.setInt1(true); //Enable interrupts
          Serial.println("Begin Interrupt Scanning");
        }
        else if(str.endsWith("it get")) {
          
          if (digitalRead(ACCEL_INT) == LOW) { //We can either check the pin
            Serial.println("Accel int pin is low!");
          }
          if (accel.getInt1() == true) {  //Or we can poll for the interrupt via I2C //Reading int will clear it
            Serial.println("Software int!");
          }
        }
      }
      else if(str.startsWith("lora")){
        if(str.endsWith("init")) {
          vdRfDrv_Init(RF_PROFILE_BASE);
          if(bRfDrv_IsOperational()) {
            Serial.print("\nok");
          }
          else {
            Serial.print("\nerror");
          }
        }
      }
    }
  }

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
      else if(bRfDrv_RecvStatusReqBlocking(1000)) {
        vdRfDrv_SendPacket(U8_PKTVAL_ERROR);
      }
      break;
    }

    case APP_ST_REWIND: {
      if(bIsNewState()){
        vdLed_FixPurple();
        vdTimeoutSet(U32_ACTUATOR_CMD_TIME);
        vdActuator_Rewind();
        Serial.print(F("Rewinding..."));
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

    case APP_ST_READY: {
      if(bIsNewState()){
        vdLed_FixGreen();
        Serial.print(F("Ready"));
      }
      else if(bRfDrv_RecvPacketBlocking(1000, &u8DataFromCtrl)) {
        if(U8_PKTVAL_GET_STATUS == u8DataFromCtrl) {
          vdRfDrv_SendPacket(U8_PKTVAL_READY);
        }
        else if(U8_PKTVAL_TRIGGER == u8DataFromCtrl) {
          vdRfDrv_SendPacket(U8_PKTVAL_PROGRESS);
          vdSetNewState(APP_ST_TRIGGER);
        }
      }
      break;
    }

    case APP_ST_TRIGGER: 
    {
      if(bIsNewState()){
        vdLed_FixPurple();
        vdTimeoutSet(U32_ACTUATOR_CMD_TIME);
        vdActuator_Trigger();
        Serial.print(F("Trigger..."));
      }
      else if(bTimeoutExpired()) {
        vdSetNewState(APP_ST_DONE);
        vdActuator_Stop();
      }
      else if(bRfDrv_RecvStatusReqBlocking(u32TimeoutGetLeftTime() + 1)) {
        vdRfDrv_SendPacket(U8_PKTVAL_PROGRESS);
      }
      break;
    }

    case APP_ST_DONE: 
    {
      if(bIsNewState()){
        vdLed_FixGreen();
        vdTimeoutSet(2000);
        Serial.print(F("Done"));
      }
      else if(bTimeoutExpired()) {
        vdSetNewState(APP_ST_REWIND);
      }
      else if(bRfDrv_RecvStatusReqBlocking(u32TimeoutGetLeftTime() + 1)) {
        vdRfDrv_SendPacket(U8_PKTVAL_DONE);
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

