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
// Driver / Service: drvBuz
// Use: Buzzer driver.
//******************************************************************************

// INCLUDES ********************************************************************

#include "drvBuz.h"

#include "drvTime.h"
#include "libSm.h"

// DEFINES *********************************************************************

// Buzzer.
#define mBuzOn()  digitalWrite(WARNING_BUZZER, HIGH);
#define mBuzOff() digitalWrite(WARNING_BUZZER, LOW);

// TYPEDEFS ********************************************************************

typedef enum {
  buzSt_off,
  buzSt_on,
  buzSt_bip,
} buzSt_t;

// VARIABLES *******************************************************************

static sm_t sm;
static sTempo_t tempo;
static uint16_t tempoDuration_ms;

// FUNCTIONS *******************************************************************

//******************************************************************************
// Name: drvBuz_init()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvBuz_init(void)
{
  sm_initInst(&sm, buzSt_off, NULL);
  tempo.bRunning = false;
  tempoDuration_ms = 0;
}

//******************************************************************************
// Name: drvBuz_task()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvBuz_task(void)
{
  sm_processEnteringSt(&sm);
  switch(sm.curSt)
  {
    default :
    case buzSt_off:
      if(sm_isEnteringFirstSt(&sm)) {
        mBuzOff();
      }
      break;

    case buzSt_on:
      if(sm_isEnteringFirstSt(&sm)) {
        mBuzOn();
      }
      break;

    case buzSt_bip:
      if(sm_isEnteringFirstSt(&sm))
      {
        mBuzOn();
        drvTime_startTempo_1ms(&tempo, tempoDuration_ms);
      }
      else if(drvTime_isElapsed(&tempo)) {
        sm.nextSt = buzSt_off;
      }
      break;
  } //end switch.
}

//******************************************************************************
// Name: drvBuz_off()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvBuz_off(void)
{
  mBuzOff();
  sm.nextSt = buzSt_off;
}

//******************************************************************************
// Name: drvBuz_on()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvBuz_on(void)
{
  sm.nextSt = buzSt_on;
}

//******************************************************************************
// Name: drvBuz_bips_ms()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvBuz_bips_ms(uint16_t duration_ms)
{
  sm.nextSt = buzSt_bip;
  tempoDuration_ms = duration_ms;
}

//******************************************************************************
// PRIVATE *********************************************************************
//******************************************************************************

//******************************************************************************
// CALLBACK ********************************************************************
//******************************************************************************

//******************************************************************************
// DO NOT EDIT *****************************************************************
//******************************************************************************



// EOF *************************************************************************
