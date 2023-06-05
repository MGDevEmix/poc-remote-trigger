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
// Driver / Service: libSm - library state machine.
// Use: Use this library to manage state machines.
//******************************************************************************

/* Usage
 *
 */


// INCLUDES ********************************************************************

#include "libSm.h"

// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************
// VARIABLES *******************************************************************
// FUNCTIONS *******************************************************************

//******************************************************************************
// Name: sm_initInst()
// Params: pointer to the state machine, init state, pointer to the enter state
//  callback (optional - can be null).
// Return: void
// Brief: init the instance of the state machine in input with the init state and
//  eventually a callback.
//******************************************************************************
void sm_initInst(sm_t *sm, uint8_t initSt,
                 void (*pEnterStCb)(uint8_t prevSt, uint8_t enterSt))
{
  sm->prevSt = initSt;
  sm->curSt = initSt;
  sm->nextSt = initSt;
  sm->bNewSt = true;
  sm->bEnterAgain = true;
  sm->pEnterStCb = pEnterStCb;
}

//******************************************************************************
// Name: sm_getPrevSt()
// Params: pointer to the state machine.
// Return: the previous state.
// Brief: return the previous state of the state machine.
//******************************************************************************
uint8_t sm_getPrevSt(sm_t *sm) {
  return sm->prevSt;
}

//******************************************************************************
// Name: sm_getCurSt()
// Params: pointer to the state machine.
// Return: the current state.
// Brief: return the current state of the state machine.
//******************************************************************************
uint8_t sm_getCurSt(sm_t *sm) {
  return sm->curSt;
}

//******************************************************************************
// Name: sm_getNextSt()
// Params: pointer to the state machine.
// Return: the current state.
// Brief: return the next state of the state machine.
//******************************************************************************
uint8_t sm_getNextSt(sm_t *sm) {
  return sm->nextSt;
}

//******************************************************************************
// Name: sm_setNextSt()
// Params: pointer to the state machine, the next state.
// Return: void
// Brief: set the next state of the state machine.
//******************************************************************************
void sm_setNextSt(sm_t *sm, uint8_t nextSt) {
  sm->nextSt = nextSt;
}

//******************************************************************************
// Name: sm_enterAgainSt()
// Params: pointer to the state machine.
// Return: void
// Brief: enter again the current state. It will trigger sm_isEnteringAgainSt()
//  but not sm_isEnteringFirstSt().
//******************************************************************************
void sm_enterAgainSt(sm_t *sm) {
  sm->bEnterAgain = true;
}

//******************************************************************************
// Name: sm_processEnteringSt()
// Params: pointer to the state machine.
// Return: void
// Brief: Process the entering to a new state: save previous state, update current
//  state, update new state indicator and call callback if registered.
//******************************************************************************
void sm_processEnteringSt(sm_t *sm)
{
  // If next state is different than current state, save previous state,
  //  update current state and call callback if registered.
  if(sm->curSt != sm->nextSt)
  {
    sm->prevSt = sm->curSt;
    sm->curSt = sm->nextSt;
    sm->bNewSt = true;
    sm->bEnterAgain = true;
    if(NULL != sm->pEnterStCb) {
      sm->pEnterStCb(sm->prevSt, sm->curSt);
    }
  }
}

//******************************************************************************
// Name: sm_isEnteringFirstSt()
// Params: pointer to the state machine.
// Return: true if this is the first time we enter in the current state.
// Brief: return true if this is the first time we enter in the current state.
//******************************************************************************
bool sm_isEnteringFirstSt(sm_t *sm)
{
  if(sm->bNewSt) {
    sm->bNewSt = false;
    return true;
  }
  return false;
}

//******************************************************************************
// Name: sm_isEnteringAgainSt()
// Params: pointer to the state machine.
// Return: true if we enter again in the current state.
// Brief: return true if this is more than first time we enter in the current state.
//******************************************************************************
bool sm_isEnteringAgainSt(sm_t *sm)
{
  if(sm->bEnterAgain) {
    sm->bEnterAgain = false;
    return true;
  }
  return false;
}

//******************************************************************************
// Name: sm_isExitingSt()
// Params: pointer to the state machine.
// Return: true if the state machine will leave the current state.
// Brief: return true if the state machine will leave the current state.
//******************************************************************************
bool sm_isExitingSt(sm_t *sm) {
  return (sm->curSt != sm->nextSt);
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
