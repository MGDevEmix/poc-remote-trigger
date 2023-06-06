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

#ifndef DRVBUZ_H_
#define DRVBUZ_H_

#ifdef	__cplusplus
extern "C" {
#endif

// INCLUDES ********************************************************************

#include "common_def.h"

// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************
// MACROS **********************************************************************
// EXTERNS *********************************************************************
// PROTOTYPES ******************************************************************

  // Init / task.
  void drvBuz_init(void);
  void drvBuz_task(void);

  // Command.
  void drvBuz_off(void);
  void drvBuz_on(void);
  void drvBuz_bips_ms(uint16_t duration_ms);

#ifdef	__cplusplus
}
#endif

#endif

// EOF *************************************************************************
