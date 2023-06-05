//******************************************************************************
//* Projet : Remote
//* All Rights Reserved 2023 DevMG
//* This program contains proprietary information which is a trade
//* secret of DevMG and/or its affiliates and also is protected as
//* an unpublished work under applicable Copyright laws. Recipient is
//* to retain this program in confidence and is not permitted to use or
//* make copies thereof other than as permitted in a written agreement
//* with DevMG, unless otherwise expressly allowed by applicable laws
//* DevMG - 2 Impasse d'Amsterdam 49460 Montreuil-Juigne
//******************************************************************************

#ifndef DRVTIME_H_
#define DRVTIME_H_

#ifdef	__cplusplus
extern "C" {
#endif

// INCLUDES ********************************************************************

#include "common_def.h"

// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************

  typedef struct
  {
    uint32_t u32StartTick;
    uint32_t u32DurationTicks;
    bool bRunning;
  } sTimeout_t;

  typedef struct
  {
    uint32_t u32StartTick;
    uint32_t u32DurationTicks;
    bool bRunning;
  } sTempo_t; // Must be exactly same to sTimeout_t!

// MACROS **********************************************************************
// EXTERNS *********************************************************************
// PROTOTYPES ******************************************************************

  // Init / task.
  void drvTime_init(void);
  void drvTime_task(void);

  // Blocking tempo
  void drvTime_wait_1ms(uint32_t u32Wait_1ms);
  void drvTime_wait_1us(uint32_t u32Wait_1us);
  void drvTime_wait_1cy(uint32_t u32Wait_1cy);

  // Timeout
  void drvTime_startTimeout_1ms(sTimeout_t* psTimeout, uint32_t u32Timeout_1ms);
  void drvTime_stopTimeout(sTimeout_t* psTimeout);
  bool drvTime_isTimedOut(sTimeout_t* psTimeout);
  void drvTime_restartTimeout(sTimeout_t* psTimeout);
  uint32_t drvTime_getTimeElapsed_1ms(sTimeout_t* psTimeout);
  uint32_t drvTime_getTimeLeft_1ms(sTimeout_t* psTimeout);

  // Tempo
  void drvTime_startTempo_1ms(sTempo_t* psTempo, uint32_t u32Tempo_1ms);
  void drvTime_stopTempo(sTempo_t* psTempo);
  bool drvTime_isElapsed(sTempo_t* psTempo);
  void drvTime_restartTempo(sTempo_t* psTempo);

  // Callback registration.
  bool drvTimer_registerCallback_1ms(void (*pCallback)(void));
  bool drvTimer_registerCallback_1sec(void (*pCallback)(void));

#ifdef	__cplusplus
}
#endif

#endif

// EOF *************************************************************************
