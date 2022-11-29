#include "led.h"

#include <Adafruit_DotStar.h>

Adafruit_DotStar rgbLed(1, 41, 40, DOTSTAR_BGR);

void vdLed_Init(void)
{
  rgbLed.begin(); // Initialize pins for output
  rgbLed.setBrightness(10); // 10% because consume too much power.
  rgbLed.setPixelColor(0, 0, 0, 0); // Off.
  rgbLed.show(); 
}

void vdLed_Off(void)
{
  rgbLed.setPixelColor(0, 0, 0, 0);
  rgbLed.show(); 
}

void vdLed_FixRed(void)
{
  rgbLed.setPixelColor(0, 255, 0, 0);
  rgbLed.show(); 
}

void vdLed_FixBlue(void)
{
  rgbLed.setPixelColor(0, 0, 0, 255);
  rgbLed.show(); 
}

void vdLed_FixPurple(void)
{
  rgbLed.setPixelColor(0, 148, 0, 211);
  rgbLed.show(); 
}

void vdLed_FixGreen(void)
{
  rgbLed.setPixelColor(0, 0, 255, 0);
  rgbLed.show(); 
}

void vdLed_FixOrange(void)
{
  rgbLed.setPixelColor(0, 255, 165, 0);
  rgbLed.show(); 
}