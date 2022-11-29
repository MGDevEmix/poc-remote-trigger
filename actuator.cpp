#include "actuator.h"

#include "pins.h"

void vdActuator_Init(void)
{
  digitalWrite(PIN_CHA_DIR, LOW);
  pinMode(PIN_CHA_DIR, OUTPUT);
  digitalWrite(PIN_CHA_PWM, LOW);
  pinMode(PIN_CHA_PWM, OUTPUT);
  digitalWrite(PIN_CHA_BRAKE, LOW);
  pinMode(PIN_CHA_BRAKE, OUTPUT);
  pinMode(PIN_CHA_CURR, INPUT);
  analogWrite(PIN_CHA_PWM, 0);
}

void vdActuator_Rewind(void)
{
  digitalWrite(PIN_CHA_DIR, LOW);
  analogWrite(PIN_CHA_PWM, 255);
}

void vdActuator_Trigger(void)
{
  digitalWrite(PIN_CHA_DIR, HIGH);
  analogWrite(PIN_CHA_PWM, 255);
}

void vdActuator_Stop(void)
{
  digitalWrite(PIN_CHA_DIR, LOW);
  analogWrite(PIN_CHA_PWM, 0);
}