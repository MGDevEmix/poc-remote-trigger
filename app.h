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

#ifndef __APP_H__
#define __APP_H__

// INCLUDES ********************************************************************

#include <Arduino.h>

#ifdef __cplusplus
 extern "C" {
#endif



void app_init(void);
void vdApp_Task(void);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __APP_H__