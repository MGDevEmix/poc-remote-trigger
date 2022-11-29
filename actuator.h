#ifndef __ACTUATOR_H__
#define __ACTUATOR_H__

#include <Arduino.h>

#ifdef __cplusplus
 extern "C" {
#endif

void vdActuator_Init(void);
void vdActuator_Rewind(void);
void vdActuator_Trigger(void);
void vdActuator_Stop(void);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __ACTUATOR_H__