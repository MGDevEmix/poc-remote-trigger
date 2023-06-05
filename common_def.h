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

#ifndef COMMONDEF_H_
#define COMMONDEF_H_

#ifdef	__cplusplus
extern "C" {
#endif

// INCLUDES ********************************************************************

// Common libraries
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // For isPrint() function.

// Common arduino.
#include <Arduino.h>
#include "pins.h"

// DEFINES *********************************************************************

#define STR_APP_VERSION "V0.00"

#define CHAR_SOH ( 1) // 0x01
#define CHAR_EOT ( 4) // 0x04
#define CHAR_CR  (13) // 0x0D
#define CHAR_LF  (10) // 0x0A

// TYPEDEFS ********************************************************************
// MACROS **********************************************************************
// EXTERNS *********************************************************************
// PROTOTYPES ******************************************************************

#ifdef	__cplusplus
}
#endif

#endif

// EOF *************************************************************************
