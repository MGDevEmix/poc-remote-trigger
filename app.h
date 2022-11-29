#ifndef __APP_H__
#define __APP_H__

#include <Arduino.h>

#ifdef __cplusplus
 extern "C" {
#endif

void vdApp_Init(void);
void vdApp_Task(void);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __APP_H__