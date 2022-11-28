#ifndef __LED_H__
#define __LED_H__

#include <Arduino.h>

#ifdef __cplusplus
 extern "C" {
#endif

void vdLed_Init(void);
void vdLed_Off(void);
void vdLed_FixRed(void);
void vdLed_FixBlue(void);
void vdLed_FixPurple(void);
void vdLed_FixGreen(void);
void vdLed_FixOrange(void);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __LED_H__