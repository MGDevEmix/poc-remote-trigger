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

#ifndef LIBSM_H_
#define LIBSM_H_

#ifdef	__cplusplus
extern "C" {
#endif

// INCLUDES ********************************************************************

#include "common_def.h"

// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************

typedef struct {
  uint8_t prevSt;   // Previous state.
  uint8_t curSt;    // Current state.
  uint8_t nextSt;   // Next state.
  bool bNewSt;      // Indicates a new state.
  bool bEnterAgain; // Indicates the current state is entered again.
  void (*pEnterStCb)(uint8_t prevSt, uint8_t enterSt); // Pointer to the callback when entering new state.
} sm_t;

// MACROS **********************************************************************
// EXTERNS *********************************************************************
// PROTOTYPES ******************************************************************

void    sm_initInst(sm_t *sm, uint8_t initSt,
                    void (*pEnterStCb)(uint8_t prevSt, uint8_t enterSt));
uint8_t sm_getPrevSt(sm_t *sm);
uint8_t sm_getCurSt(sm_t *sm);
uint8_t sm_getNextSt(sm_t *sm);
void    sm_setNextSt(sm_t *sm, uint8_t nextSt);
void    sm_enterAgainSt(sm_t *sm);
void    sm_processEnteringSt(sm_t *sm);
bool    sm_isEnteringFirstSt(sm_t *sm);
bool    sm_isEnteringAgainSt(sm_t *sm);
bool    sm_isExitingSt(sm_t *sm);

#ifdef	__cplusplus
}
#endif

#endif

// EOF *************************************************************************
