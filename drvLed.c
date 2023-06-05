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
  ledSt_blink_on,
  ledSt_blink_off,
  ledSt_flash_on,
} ledSt_t;

// VARIABLES *******************************************************************

static sm_t sm;
static sTempo_t tempo;
static ledColor_t currentColor;
static uint16_t tempoOn_ms;
static uint16_t tempoOff_ms;

// FUNCTIONS *******************************************************************

/*static*/ void setColor(ledColor_t color);
void vdLed_Init(void);
void vdLed_Off(void);
void vdLed_FixRed(void);
void vdLed_FixBlue(void);
void vdLed_FixPurple(void);
void vdLed_FixGreen(void);
void vdLed_FixOrange(void);

//******************************************************************************
// Name: drvLed_init()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvLed_init(void)
{
  sm_initInst(&sm, ledSt_off, NULL);
  tempo.bRunning = false;
  currentColor = ledColor_off;
  tempoOn_ms = 0;
  tempoOff_ms = 0;
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

    case ledSt_blink_on:
      if(sm_isEnteringFirstSt(&sm))
      {
        setColor(currentColor);
        drvTime_startTempo_1ms(&tempo, tempoOn_ms);
      }
      else if(drvTime_isElapsed(&tempo)) {
        sm.nextSt = ledSt_blink_off;
      }
      break;

    case ledSt_blink_off:
      if(sm_isEnteringFirstSt(&sm))
      {
        setColor(ledColor_off);
        drvTime_startTempo_1ms(&tempo, tempoOff_ms);
      }
      else if(drvTime_isElapsed(&tempo)) {
        sm.nextSt = ledSt_blink_on;
      }
      break;

    case ledSt_flash_on:
      if(sm_isEnteringFirstSt(&sm))
      {
        setColor(currentColor);
        drvTime_startTempo_1ms(&tempo, tempoOn_ms);
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
  tempoOn_ms = timeOn_ms;
  tempoOff_ms = timeOff_ms;
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
  tempoOn_ms = timeOn_ms;
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
