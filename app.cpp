//******************************************************************************
//* Projet : Base
//* All Rights Reserved 2023 DevMG
//* This program contains proprietary information which is a trade
//* secret of DevMG and/or its affiliates and also is protected as
//* an unpublished work under applicable Copyright laws. Recipient is
//* to retain this program in confidence and is not permitted to use or
//* make copies thereof other than as permitted in a written agreement
//* with DevMG, unless otherwise expressly allowed by applicable laws
//* DevMG - 2 Impasse d'Amsterdam 49460 Montreuil-Juigne
//******************************************************************************

// INCLUDES ********************************************************************

#include "app.h"
#include "pins.h"

// Arduino specific.
#include "api/Common.h"
#include <Wire.h>
#include <ArduinoLowPower.h>

// C++ code.
#include "SparkFun_LIS2DH12.h"

// Portable C code.
#include "drvLed.h"
#include "drvBuz.h"
#include "drvTime.h"
#include "libSm.h"

// Non portable C code.
#include "rf_drv.h"
#include "timeout.h"
#include "actuator.h"

// DEFINES *********************************************************************

#define ACTUATOR_CMD_TIME ((uint32_t)4000)

// Led pulses.
#define LED_PULSE_ON (200)
#define LED_PULSE_OFF_SLOW_BLINK (1000)
#define LED_PULSE_OFF_FAST_BLINK (200)

#define WARNING_RESET (200)

#define IMEAS_OVERCURRENT (0.6)//(2.0) // xA.
#define IMEAS_DECIMATION (0.4 / 100.0) // 0.4%.

// TYPEDEFS ********************************************************************

enum appSt_t {
  appSt_init,
  appSt_error,
  appSt_rewind,
  appSt_ready,
  appSt_trigger,
  appSt_done,
};

// VARIABLES *******************************************************************

// SM vars.
static sm_t sm;
static sTimeout_t timeout;
static sTempo_t tempo;
static uint8_t u8DataFromCtrl; // $TODO: to del ?

// Accel vars.
static SPARKFUN_LIS2DH12 accel;       //Create instance

// Imeas var.
double iMeas_acc;

// FUNCTIONS *******************************************************************

// Imeas.
static void iMeas_init(void);
static bool iMeas_processOvercurrent(void);
// Debug.
static void parseString(String str);
static char* getStrAppSt(appSt_t appSt);
// Callbacks.
static void irqBut1WakeUp(void);
static void appInNewStCallback(uint8_t prevSt, uint8_t enterSt);

//******************************************************************************
// Name: app_init()
// Params: void
// Return: void
// Brief: 
//******************************************************************************
void app_init(void)
{
  // Init Hw.
  pinMode(WARNING_BUZZER, OUTPUT); //$TODO : init pins.

  // Low power.
  LowPower.attachInterruptWakeup(RF_DIO1, irqBut1WakeUp, RISING);

  // Init vars.
  Serial.print(F("Base ready"));
  Serial.println();
  Serial.setTimeout(100);
  sm_initInst(&sm, appSt_init, &appInNewStCallback);
  timeout.bRunning = false;
  tempo.bRunning = false;
}

//******************************************************************************
// Name: vdApp_Task()
// Params: void
// Return: void
// Brief: 
//******************************************************************************
void vdApp_Task(void)
{
  // Manage Serial.
  if(Serial.available()) {
    static String str;
    str = Serial.readString();
    parseString(str);
  }

  // Manage state machine.
  sm_processEnteringSt(&sm);
  switch(sm.curSt)
  {
    default:
    case appSt_init: 
    {
      if(bRfDrv_IsOperational()) {
        sm.nextSt = appSt_rewind;
      }
      else {
        sm.nextSt = appSt_error;
      }
      break;
    }

    case appSt_error: 
    {
      if(sm_isEnteringFirstSt(&sm)){
        drvLed_blink(ledColor_red, LED_PULSE_ON, LED_PULSE_OFF_FAST_BLINK);
        drvBuz_bips_ms(3000);
        vdRfDrv_SetRxContinuous();  
        drvTime_startTimeout_1ms(&timeout, 5000);
      }
      else if(bRfDrv_RecvStatusOrDiscover()) {
        vdRfDrv_SendPacketAndRxContinuous(U8_PKTVAL_ERROR);
      }
      else if(drvTime_isTimedOut(&timeout)) {
        drvLed_off();
        drvBuz_off();
        vdRfDrv_SetSleep();
        LowPower.sleep();
        // Zzz.
      }
      break;
    }

    case appSt_rewind: {
      if(sm_isEnteringFirstSt(&sm)){
        drvLed_off();
        drvBuz_off(); // Reset warning.
        drvTime_startTempo_1ms(&tempo, WARNING_RESET);
        drvTime_startTimeout_1ms(&timeout, ACTUATOR_CMD_TIME);
        vdActuator_Rewind();
        vdRfDrv_SetRxContinuous();
        iMeas_init();
      }
      else if(drvTime_isElapsed(&tempo) && sm_isEnteringAgainSt(&sm)) {
        drvLed_blink(ledColor_purple, LED_PULSE_ON, LED_PULSE_OFF_SLOW_BLINK);
      }
      else if(drvTime_isTimedOut(&timeout) || iMeas_processOvercurrent()) {
        sm.nextSt = appSt_ready;
        vdActuator_Stop();
      }
      else if(bRfDrv_RecvStatusOrDiscover()) {
        vdRfDrv_SendPacketAndRxContinuous(U8_PKTVAL_PROGRESS);
      }
      break;
    }

    case appSt_ready: {
      if(sm_isEnteringFirstSt(&sm)){
        drvLed_off();
        drvBuz_off(); // Reset warning.
        drvTime_startTempo_1ms(&tempo, WARNING_RESET);
        drvTime_startTimeout_1ms(&timeout, 15000);
        vdRfDrv_SetRxContinuous();
      }
      else if(drvTime_isElapsed(&tempo) && sm_isEnteringAgainSt(&sm))
      {
        drvLed_blinkOnAndOff(ledColor_green, 4, LED_PULSE_ON, LED_PULSE_OFF_FAST_BLINK, 10000);
        drvBuz_bips_ms(1000);
      }
      else if(bRfDrv_RecvPacket(&u8DataFromCtrl)) 
      {
        if(U8_PKTVAL_GET_STATUS == u8DataFromCtrl) {
          vdRfDrv_SendPacketAndRxContinuous(U8_PKTVAL_READY);
        }
        else if(U8_PKTVAL_DISCOVER == u8DataFromCtrl) {
          vdRfDrv_SendPacketAndRxContinuous(U8_PKTVAL_READY);
          sm_setNextSt(&sm, appSt_ready); // Blink again.
        }
        else if(U8_PKTVAL_TRIGGER == u8DataFromCtrl) {
          vdRfDrv_SendPacketAndRxContinuous(U8_PKTVAL_PROGRESS);
          sm.nextSt = appSt_trigger;
        }
      }
      else if(drvTime_isTimedOut(&timeout)) {
        drvLed_off();
        drvBuz_off();
        //vdRfDrv_SetSleep(); We want to be waken up by RF.
        LowPower.sleep();
        // Zzz.
      }
      break;
    }

    case appSt_trigger: 
    {
      if(sm_isEnteringFirstSt(&sm)){
        drvLed_off();
        drvBuz_off(); // Reset warning.
        drvTime_startTempo_1ms(&tempo, WARNING_RESET);
        drvTime_startTimeout_1ms(&timeout, ACTUATOR_CMD_TIME);
        vdActuator_Trigger();
        vdRfDrv_SetRxContinuous();
        iMeas_init();
      }
      else if(drvTime_isElapsed(&tempo) && sm_isEnteringAgainSt(&sm)) {
        drvLed_blink(ledColor_orange, LED_PULSE_ON, LED_PULSE_OFF_SLOW_BLINK);
        drvBuz_on();
      }
      else if(drvTime_isTimedOut(&timeout) || iMeas_processOvercurrent()) {
        sm.nextSt = appSt_done;
        vdActuator_Stop();
      }
      else if(bRfDrv_RecvStatusOrDiscover()) {
        vdRfDrv_SendPacketAndRxContinuous(U8_PKTVAL_PROGRESS);
      }
      break;
    }

    case appSt_done: 
    {
      if(sm_isEnteringFirstSt(&sm)){
        drvLed_on(ledColor_green);
        drvBuz_off();
        vdTimeoutSet(2000);
        vdRfDrv_SetRxContinuous();
      }
      else if(bTimeoutExpired()) {
        sm.nextSt = appSt_rewind;
      }
      else if(bRfDrv_RecvStatusOrDiscover()) {
        vdRfDrv_SendPacketAndRxContinuous(U8_PKTVAL_PROGRESS);
      }
      break;
    }

  } //end switch
}

//******************************************************************************
// PRIVATE *********************************************************************
//******************************************************************************

//******************************************************************************
// Name: iMeas_init()
// Params: 
// Return: 
// Brief: init the overcurrent measurement.
//******************************************************************************
void iMeas_init(void)
{
  analogReadResolution(12); // Set 12bit resolution.
  iMeas_acc = 0;
}

//******************************************************************************
// Name: iMeas_processOvercurrent()
// Params: 
// Return: true if overccurent detected.
// Brief: process current measurement. Return the result of measurement.
//******************************************************************************
bool iMeas_processOvercurrent(void)
{
  static uint16_t u16Sample;
  static double current_A;
  u16Sample = analogRead(PIN_A0);
  iMeas_acc = iMeas_acc * (1.0 - IMEAS_DECIMATION); // Keep (1-x)% of accumulator.
  iMeas_acc += ((double)u16Sample) * IMEAS_DECIMATION; // Add x% of new sample.
  current_A = iMeas_acc / 4096.0 * 2.0;
  Serial.print(" ");
  //Serial.print(u16Sample); // Current sample.
  //Serial.print(iMeas_acc); // Accumulator (sample filtered).
  Serial.print(current_A); // Real amp value from accumulator (/4096 *3.3V /3.3V *2.0A).
  if(current_A > IMEAS_OVERCURRENT) {
    return true;
  }
  return false;
}

//******************************************************************************
// Name: parseString()
// Params: 
// Return: 
// Brief: 
//******************************************************************************
void parseString(String str)
{
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
    else if(str.startsWith("test"))
    {
      Serial.print("\n Test start...");
      drvLed_blink(ledColor_blue, 100, 200);
      Serial.print("done");
    }
  }
}

//******************************************************************************
// Name: getStrAppSt()
// Params: appSt_t appSt
// Return: string of the state.
// Brief: callback when app enters a new state.
//******************************************************************************
char* getStrAppSt(appSt_t appSt)
{
  switch(appSt)
  {
    default: return "?";
    case appSt_init: return "init";
    case appSt_error: return "error";
    case appSt_rewind: return "rewind";
    case appSt_ready: return "ready";
    case appSt_trigger: return "trigger";
    case appSt_done: return "done";
  }
}

//******************************************************************************
// CALLBACK ********************************************************************
//******************************************************************************

//******************************************************************************
// Name: appInNewStCallback()
// Params: uint8_t prevSt, uint8_t enterSt
// Return: void
// Brief: callback when app enters a new state.
//******************************************************************************
void appInNewStCallback(uint8_t prevSt, uint8_t enterSt)
{
  Serial.print("\n App state [");
  Serial.print(getStrAppSt((appSt_t)prevSt));
  Serial.print("] -> [");
  Serial.print(getStrAppSt((appSt_t)enterSt));
  Serial.print("]");
}

void irqBut1WakeUp(void) {
  // This function will be called once on device wakeup
  // You can do some little operations here (like changing variables which will be used in the loop)
  // Remember to avoid calling delay() and long running functions since this functions executes in interrupt context
}

