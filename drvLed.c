//******************************************************************************
//* Project : Remote
//* All Rights Reserved 2023 DevMG
//* This program contains proprietary information which is a trade
//* secret of DevMG and/or its affiliates and also is protected as
//* an unpublished work under applicable Copyright laws. Recipient is
//* to retain this program in confidence and is not permitted to use or
//* make copies thereof other than as permitted in a written agreement
//* with DevMG, unless otherwise expressly allowed by applicable laws
//* DevMG - 2 Impasse d'Amsterdam 49460 Montreuil-Juigne
//******************************************************************************

//******************************************************************************
// Driver / Service:
// Use:
//******************************************************************************

// INCLUDES ********************************************************************

#include "drvLed.h"

#include "drvTime.h"
#include "libSm.h"

// DEFINES *********************************************************************

// TYPEDEFS ********************************************************************

typedef enum {
  ledSt_off,
  ledSt_on,
  ledSt_blink_on,
  ledSt_blink_off,
  ledSt_flash_on,
} ledSt_t;

// VARIABLES *******************************************************************

static sm_t sm;
static sTempo_t tempo;
static ledColor_t currentColor;
static uint16_t tempoBlinkOn_ms;
static uint16_t tempoBlinkOff_ms;
static uint16_t tempoFlashOn_ms;
static uint8_t blinkCnt;

// FUNCTIONS *******************************************************************

static void setColor(ledColor_t color);
static void vdLed_Init(void);
static void vdLed_Off(void);
static void vdLed_FixRed(void);
static void vdLed_FixBlue(void);
static void vdLed_FixPurple(void);
static void vdLed_FixGreen(void);
static void vdLed_FixOrange(void);

//******************************************************************************
// Name: drvLed_init()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvLed_init(void)
{
  // Init hw.
  pinMode(LED_R, OUTPUT);   
  pinMode(LED_G, OUTPUT);  
  pinMode(LED_B, OUTPUT);  
  
  // Init vars.
  sm_initInst(&sm, ledSt_off, NULL);
  tempo.bRunning = false;
  currentColor = ledColor_off;
  tempoBlinkOn_ms = 0;
  tempoBlinkOff_ms = 0;
  tempoFlashOn_ms = 0;
  blinkCnt = 0;
}

//******************************************************************************
// Name: drvLed_task()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvLed_task(void)
{
  sm_processEnteringSt(&sm);
  switch(sm.curSt)
  {
    default :
    case ledSt_off:
      if(sm_isEnteringFirstSt(&sm)) {
        setColor(ledColor_off);
      }
      break;

    case ledSt_on:
      if(sm_isEnteringFirstSt(&sm)) {
        setColor(currentColor);
      }
      break;

    case ledSt_blink_on:
      if(sm_isEnteringFirstSt(&sm))
      {
        setColor(currentColor);
        drvTime_startTempo_1ms(&tempo, tempoBlinkOn_ms);
      }
      else if(drvTime_isElapsed(&tempo)) {
        sm.nextSt = ledSt_blink_off;
      }
      break;

    case ledSt_blink_off:
      if(sm_isEnteringFirstSt(&sm))
      {
        setColor(ledColor_off);
        drvTime_startTempo_1ms(&tempo, tempoBlinkOff_ms);
      }
      else if(drvTime_isElapsed(&tempo)) {
        if(blinkCnt == 255) {
          sm.nextSt = ledSt_blink_on;
        }
        else if(blinkCnt > 0)
        {
          blinkCnt--;
          if(blinkCnt > 0){
            sm.nextSt = ledSt_blink_on;
          }
          else
          {
            if(tempoFlashOn_ms > 0) {
              sm.nextSt = ledSt_flash_on;
            }
            else {
              sm.nextSt = ledSt_off;
            }
          }
        }
      }
      break;

    case ledSt_flash_on:
      if(sm_isEnteringFirstSt(&sm))
      {
        setColor(currentColor);
        drvTime_startTempo_1ms(&tempo, tempoFlashOn_ms);
      }
      else if(drvTime_isElapsed(&tempo)) {
        sm.nextSt = ledSt_off;
      }
      break;

  } //end switch.
}

//******************************************************************************
// Name: drvLed_off()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvLed_off(void)
{
  sm.nextSt = ledSt_off;
  tempo.bRunning = false;
  currentColor = ledColor_off;

  // Do it right now.
  setColor(ledColor_off);
}

//******************************************************************************
// Name: drvLed_on()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvLed_on(ledColor_t color)
{
  sm.nextSt = ledSt_on;
  tempo.bRunning = false;
  currentColor = color;

  // Do it right now.
  setColor(color);
}

//******************************************************************************
// Name: drvLed_blinkFast()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvLed_blink(ledColor_t color, uint16_t timeOn_ms, uint16_t timeOff_ms)
{
  sm.nextSt = ledSt_blink_on;
  currentColor = color;
  tempoBlinkOn_ms = timeOn_ms;
  tempoBlinkOff_ms = timeOff_ms;
  blinkCnt = 255; // Infinite.
}

//******************************************************************************
// Name: drvLed_flash()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvLed_flash(ledColor_t color, uint16_t timeOn_ms)
{
  sm.nextSt = ledSt_flash_on;
  currentColor = color;
  tempoFlashOn_ms = timeOn_ms;
}

//******************************************************************************
// Name: drvLed_blinkOnAndOff()
// Params: void
// Return: void
// Brief: Blinks a color for a number of blink count, with specified on and off
//  time. Then turn on light during a time and then turn off light.
//******************************************************************************
void drvLed_blinkOnAndOff(ledColor_t color, 
          uint8_t blinksCnt, uint16_t timeOnBlink_ms, uint16_t timeOffBlink_ms, 
          uint16_t timeOnFix_ms) 
{
  sm.nextSt = ledSt_blink_on;
  currentColor = color;
  blinkCnt = blinksCnt;
  tempoBlinkOn_ms = timeOnBlink_ms;
  tempoBlinkOff_ms = timeOffBlink_ms;
  tempoFlashOn_ms = timeOnFix_ms;
}

//******************************************************************************
// PRIVATE *********************************************************************
//******************************************************************************

//******************************************************************************
// Name: setColor()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void setColor(ledColor_t color)
{
  switch(color)
  {
    default: break;
    case ledColor_off:
      vdLed_Off();
      break;
    case ledColor_red:
      vdLed_FixRed();
      break;
    case ledColor_purple:
      vdLed_FixPurple();
      break;
    case ledColor_green:
      vdLed_FixGreen();
      break;
    case ledColor_orange:
      vdLed_FixOrange();
      break;
    case ledColor_blue:
      vdLed_FixBlue();
      break;
  }
}

void vdLed_Off(void)
{
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
}

void vdLed_FixRed(void)
{
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
}

void vdLed_FixBlue(void)
{
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, HIGH);
}

void vdLed_FixPurple(void)
{
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, HIGH);
}

void vdLed_FixGreen(void)
{
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, LOW);
}

void vdLed_FixOrange(void)
{
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, LOW);
}

//******************************************************************************
// CALLBACK ********************************************************************
//******************************************************************************

//******************************************************************************
// DO NOT EDIT *****************************************************************
//******************************************************************************



// EOF *************************************************************************
