#include "api/Common.h"
#include "led.h"

#include "pins.h"

//#include <Adafruit_DotStar.h>

//Adafruit_DotStar rgbLed(1, 41, 40, DOTSTAR_BGR);

void vdLed_Init(void)
{
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
  pinMode(LED_R, OUTPUT);   
  pinMode(LED_G, OUTPUT);  
  pinMode(LED_B, OUTPUT);  
}

void vdLed_Off(void)
{
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
}

void vdLed_FixRed(void)
{
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
}

void vdLed_FixBlue(void)
{
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, HIGH);
}

void vdLed_FixPurple(void)
{
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, HIGH);
}

void vdLed_FixGreen(void)
{
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, LOW);
}

void vdLed_FixOrange(void)
{
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, LOW);
}