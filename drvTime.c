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

#include "drvTime.h"

//#include "tim.h"

// DEFINES *********************************************************************

#define MAX_NB_CALLBACKS (5)

//#define LOCAL_HANDLE (&htim21)

// TYPEDEFS ********************************************************************
// VARIABLES *******************************************************************

// Private
static void (*apCallbacks_1ms[MAX_NB_CALLBACKS]) (void);  // Array of callbacks registered to 1ms elapsed.
static void (*apCallbacks_1sec[MAX_NB_CALLBACKS]) (void); // Array of callbacks registered to 1s elapsed.
static uint8_t numCallbacks_1ms;
static uint8_t numCallbacks_1sec;
static uint32_t prevTick;

// FUNCTIONS *******************************************************************

// Private
uint32_t getNbTicksElapsed(sTimeout_t* psTimeout);

//******************************************************************************
// Name: drvTime_init()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvTime_init(void)
{
  memset(apCallbacks_1ms, 0, sizeof(apCallbacks_1ms));
  memset(apCallbacks_1sec, 0, sizeof(apCallbacks_1sec));
  numCallbacks_1ms = 0;
  numCallbacks_1sec = 0;
  prevTick = 0;
}

//******************************************************************************
// Name: drvTime_task()
// Params: void
// Return: void
// Brief:
//******************************************************************************
void drvTime_task(void)
{
  static uint32_t newTick;
  static uint8_t i;

  newTick = millis();
  if(prevTick != newTick)
  {
    // 1ms elapsed => call callbacks 1ms.
    for(i = 0; i < numCallbacks_1ms; i++) {
      (*apCallbacks_1ms[i])();
    }

    if((prevTick % 1000) > (newTick % 1000))
    {
      // 1sec elapsed => call callbacks 1sec.
      for(i = 0; i < numCallbacks_1sec; i++) {
        (*apCallbacks_1sec[i])();
      }
    }
    prevTick = newTick;
  }
}

/*************************************************************************//**
  * @fn     drvTime_wait_1cy
  * @param  uint32_t u32Wait_1cy
  * @retval void
  * @brief  Fonction bloquante attendant AU MOINS x cycles CPU avant de rendre la main
 ****************************************************************************** */
void drvTime_wait_1cy(uint32_t u32Wait_1cy)
{
  volatile uint32_t u32Cnt;
  u32Cnt = u32Wait_1cy;
  while(u32Cnt > 0)
    u32Cnt--;
}

//******************************************************************************
// Name: drvTime_wait_1us()
// Params: uint32_t u32Wait_1us
// Return: void
// Brief: blocking function waiting AT LEAST x us before leaving.
//        Exp measure: drvTime_wait_1us(10)   -> 14-16us.    Speed optm.
//                     drvTime_wait_1us(100)  -> 109-111us.
//                     drvTime_wait_1us(1000) -> 1064us.
//******************************************************************************
void drvTime_wait_1us(uint32_t u32Wait_1us)
{/*
  HAL_TIM_Base_Start(LOCAL_HANDLE);
  LOCAL_HANDLE->Instance->CNT = 0;
  while(LOCAL_HANDLE->Instance->CNT <= u32Wait_1us);*/
}

/**//*************************************************************************
  * @fn     drvTime_wait_1ms
  * @param  uint32_t u32Wait_1ms
  * @retval void
  * @brief  Fonction bloquante attendant x ms (-1/+0 ms) avant de rendre la main.
**//*************************************************************************/
void drvTime_wait_1ms(uint32_t u32Wait_1ms)
{
  sTimeout_t sTimeout;
  drvTime_startTimeout_1ms(&sTimeout, u32Wait_1ms);
  while(!drvTime_isTimedOut(&sTimeout))
    {;}
}

//******************************************************************************
// Name: drvTime_startTimeout_1ms()
// Params: sTimeout_t* psTimeout, uint32_t u32Timeout1ms
// Return: void
// Brief: Start a timeout with the duration in parameter.
//******************************************************************************
void drvTime_startTimeout_1ms(sTimeout_t* psTimeout, uint32_t u32Timeout_1ms)
{
  // 1 tick = 1 ms.
  psTimeout->u32StartTick = millis();
  psTimeout->u32DurationTicks = u32Timeout_1ms;
  psTimeout->bRunning = true;
}

/**//*************************************************************************
  * @fn     drvTime_stopTimeout
  * @param  sTimeout_t* psTimeout
  * @retval void
  * @brief  Arrete un timeout.
**//*************************************************************************/
void drvTime_stopTimeout(sTimeout_t* psTimeout) {
  psTimeout->bRunning = false;
}

/**//*************************************************************************
  * @fn     drvTime_isTimedOut
  * @param  sTimeout_t* psTimeout
  * @retval bool
  * @brief  Retourne true si le timeout est atteint.
**//*************************************************************************/
bool drvTime_isTimedOut(sTimeout_t* psTimeout)
{
  if(psTimeout->bRunning)
  {
    if(getNbTicksElapsed(psTimeout) >= psTimeout->u32DurationTicks)
    {
      psTimeout->bRunning = false;
      return true;
    }
    else
      return false;
  }
  else
    return true;
}

/**//*************************************************************************
  * @fn     SrvTime_restartTimeout
  * @param  sTimeout_t* psTimeout
  * @retval void
  * @brief  Redemarre un timeout.
**//*************************************************************************/
void drvTime_restartTimeout(sTimeout_t* psTimeout) {
  drvTime_startTimeout_1ms(psTimeout, psTimeout->u32DurationTicks);
}

/*************************************************************************//**
  * @fn     drvTime_getTimeElapsed_1ms
  * @param  sTimeout_t* psTimeout
  * @retval uint32_t
  * @brief  Revoie le temps ecoule en ms depuis le demarrage du timeout.
 ****************************************************************************** */
uint32_t drvTime_getTimeElapsed_1ms(sTimeout_t* psTimeout) {

  return getNbTicksElapsed(psTimeout); // 1 tick = 1 ms.
}

/*************************************************************************//**
  * @fn     drvTime_getTimeLeft_1ms
  * @param  sTimeout_t* psTimeout
  * @retval uint32_t
  * @brief  Renvoie le temps restant du timeout.
 ****************************************************************************** */
uint32_t drvTime_getTimeLeft_1ms(sTimeout_t* psTimeout) {
  return psTimeout->u32DurationTicks - getNbTicksElapsed(psTimeout); // 1 tick = 1 ms.
}

/**//*************************************************************************
  * @fn     drvTime_startTempo_1ms
  * @param  sTempo_t* psTempo, uint32_t u32Tempo_1ms
  * @retval void
  * @brief  Demarre une tempo en ms.
**//*************************************************************************/
void drvTime_startTempo_1ms(sTempo_t* psTempo, uint32_t u32Tempo_1ms) {
  drvTime_startTimeout_1ms((sTimeout_t*)psTempo, u32Tempo_1ms);
}

/**//*************************************************************************
  * @fn     drvTime_stopTempo
  * @param  sTempo_t* psTempo
  * @retval void
  * @brief  Arrete la tempo.
**//*************************************************************************/
void drvTime_stopTempo(sTempo_t* psTempo) {
  drvTime_stopTimeout((sTimeout_t*)psTempo);
}

/**//*************************************************************************
  * @fn     drvTime_isElapsed
  * @param  sTempo_t* psTempo
  * @retval void
  * @brief  Indique si la tempo est ecoulee.
**//*************************************************************************/
bool drvTime_isElapsed(sTempo_t* psTempo) {
  return drvTime_isTimedOut((sTimeout_t*)psTempo);
}

/**//*************************************************************************
  * @fn     SrvTime_restartTempo
  * @param  sTempo_t* psTempo
  * @retval void
  * @brief  Redemarre une tempo.
**//*************************************************************************/
void drvTime_restartTempo(sTempo_t* psTempo) {
  drvTime_restartTimeout((sTimeout_t*)psTempo);
}

//******************************************************************************
// Name: drvTimer_registerCallback_1ms()
// Params: pCallback: pointer to callback.
// Return: void
// Brief:
//******************************************************************************
bool drvTimer_registerCallback_1ms(void (*pCallback)(void))
{
  if(numCallbacks_1ms < MAX_NB_CALLBACKS) {
    apCallbacks_1ms[numCallbacks_1ms++] = pCallback;
    return true;
  }
  return false;
}

//******************************************************************************
// Name: drvTimer_registerCallback_1sec()
// Params: pCallback: pointer to callback.
// Return: void
// Brief:
//******************************************************************************
bool drvTimer_registerCallback_1sec(void (*pCallback)(void))
{
  if(numCallbacks_1sec < MAX_NB_CALLBACKS) {
    apCallbacks_1sec[numCallbacks_1sec++] = pCallback;
    return true;
  }
  return false;
}

//******************************************************************************
// PRIVATE *********************************************************************
//******************************************************************************

/*************************************************************************//**
  * @fn     getNbTicksElapsed
  * @param  sTimeout_t* psTimeout
  * @retval uint32_t
  * @brief  Retourne le temps ecoule en nombre de ticks.
 ****************************************************************************** */
uint32_t getNbTicksElapsed(sTimeout_t* psTimeout)
{
  uint32_t u32NbTicksElapsed;

  // Si le timeout est en cours, on regarde le temps ecoule depuis son demarrage.
  // Si le temps ecoule est superieur a sa duree, le timeout est ecoule.
  if(psTimeout->bRunning)
  {
    u32NbTicksElapsed = millis() - psTimeout->u32StartTick;
    if(u32NbTicksElapsed >= psTimeout->u32DurationTicks)
      psTimeout->bRunning = false;
  }

  // Si le timeout est ecoule, on renvoie sa duree, sinon on renvoie le temps
  //  ecoule.
  if(!psTimeout->bRunning)
    return u32NbTicksElapsed = psTimeout->u32DurationTicks;

  return u32NbTicksElapsed;
}

//******************************************************************************
// CALLBACK ********************************************************************
//******************************************************************************

//******************************************************************************
// DO NOT EDIT *****************************************************************
//******************************************************************************

// EOF *************************************************************************
