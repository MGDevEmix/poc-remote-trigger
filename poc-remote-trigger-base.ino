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

//******************************************************************************
//* Hardware Information:
//* - MCU      : Arduino Zero
//* - IDE      : Arduino IDE 2.1.0
//* - COMPILER : Arduino
//******************************************************************************

// INCLUDES ********************************************************************

// Portable C code.
#include "drvLed.h"
#include "drvBuz.h"

// Non portable C code.
#include "app.h"
#include "serial_drv.h"
#include "rf_drv.h"
#include "actuator.h"

// DEFINES *********************************************************************

#define Program_Version "V0.0"

// TYPEDEFS ********************************************************************
// VARIABLES *******************************************************************
// FUNCTIONS *******************************************************************

void setup() 
{
  delay(2000);

  // Init drivers.
  vdSerialDrv_Init(Program_Version);
  vdRfDrv_Init(RF_PROFILE_BASE);
  vdActuator_Init();
  drvLed_init();
  drvBuz_init();

  // Init app.
  app_init(); 
}

void loop() {
  vdApp_Task();
  drvLed_task();
  drvBuz_task();
}


