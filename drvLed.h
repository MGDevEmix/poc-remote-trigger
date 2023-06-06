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

#ifndef DRVLED_H_
#define DRVLED_H_

#ifdef	__cplusplus
extern "C" {
#endif

// INCLUDES ********************************************************************

#include "common_def.h"

// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************

  typedef enum {
    ledColor_off,
    ledColor_red,
    ledColor_purple,
    ledColor_green,
    ledColor_orange,
    ledColor_blue,
  } ledColor_t;

// MACROS **********************************************************************
// EXTERNS *********************************************************************
// PROTOTYPES ******************************************************************

  // Init / task.
  void drvLed_init(void);
  void drvLed_task(void);

  // Commands.
  void drvLed_off(void);
  void drvLed_on(ledColor_t color);
  void drvLed_blink(ledColor_t color, uint16_t timeOn_ms, uint16_t timeOff_ms);
  void drvLed_flash(ledColor_t color, uint16_t timeOn_ms);
  void drvLed_blinkOnAndOff(ledColor_t color, uint8_t blinkCnt, uint16_t timeOnBlink_ms, uint16_t timeOffBlink_ms, uint16_t timeOnFix_ms);

#ifdef	__cplusplus
}
#endif

#endif

// EOF *************************************************************************
